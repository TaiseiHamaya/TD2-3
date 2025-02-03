#include "PlayerManager.h"

#include <cmath>

#include "Engine/Resources/Animation/NodeAnimation/NodeAnimationPlayer.h"
#include "Engine/Runtime/Input/Input.h"

#include <Application/Utility/GameUtility.h>
#include <Engine/Utility/Tools/SmartPointer.h>

void PlayerManager::initialize(Reference<const LevelLoader> level, MapchipField* mapchipField) {
	mapchipField_ = mapchipField;

	catchEffect_ = std::make_unique<AnimatedMeshInstance>();
	catchEffect_->reset_animated_mesh("CatchEffect.gltf", "", true);

	dustEmitter = eps::CreateUnique<ParticleEmitterInstance>("dust.json", 128);
	iceDustEmitter = eps::CreateUnique<ParticleEmitterInstance>("iceDust.json", 128);

	mapchipHandler = std::make_unique<MapchipHandler>();
	mapchipHandler->initialize(mapchipField_);

	child = std::make_unique<Child>();
	child->initialize(*level.ptr(), mapchipHandler.get());

	player = std::make_unique<Player>();
	player->initialize(*level.ptr(), mapchipHandler.get());
	player->set_child(child.get());

	if (player) {
		player->set_parent(false);
	}
	stageSituation = 0;
	isParent = false;

	//音関連
	holdAudio = std::make_unique<AudioPlayer>();
	holdAudio->initialize("hold.wav");
	releaseAudio = std::make_unique<AudioPlayer>();
	releaseAudio->initialize("release.wav");

	moveLogger = std::make_unique<MoveLogger>();
	moveLogger->initialize();
}

void PlayerManager::finalize() {
	player->finalize();
	child->finalize();
}

void PlayerManager::update() {
	isStackMovement = false;
	// クリアか失敗のフラグが立ってたら早期リターン
	if (stageSituation != 0) return;

	catchEffect_->begin();

	// プレイヤーと子供の位置を計算
	playerPos = player->get_translate();
	childPos = child->get_object()->world_position();
	Vector3 emitterPos = playerPos;
	emitterPos.y -= 0.5f;

	isParent = player->is_parent();//プレイヤーのくっつき状態のフラグを取得
	// マップチップ関連の更新
	mapchipHandler->update_player_on_mapchip(player.get(), child.get());
	// 入力処理を受け付ける
	handle_input();

	// 状態の更新
	player->update();
	child->update();
	catchEffect_->get_transform().set_translate({ 0.0f, 2.0f, 0.0f });

	dustEmitter->get_transform().set_translate(emitterPos);
	dustEmitter->update();

	iceDustEmitter->get_transform().set_translate(emitterPos);
	iceDustEmitter->update();

	if (player->is_stack_movement()) {
		emplace_log(player->move_start_position(), player->start_rotation());
	}

	// 親子関係の管理
	manage_parent_child_relationship();
	// 子供をプレイヤーの向かせる処理
	set_child_rotate();
	// パーティクルのオンオフの切り替え処理
	particle_update();

	// マップチップのクリア判定
	stageSituation = mapchipHandler->is_goal_reached(player.get(), child.get());
	// 子コアラを落とした
	if (child->is_falled() || player->is_falled()) {
		stageSituation = 4;
	}
	// 子を連れて親が先にゴール
	if (stageSituation == 1) {
		gameManagement_->SetClearFlag(true);
	}
	// 子供が先にゴール
	else if (stageSituation == 2) {
		gameManagement_->SetFailedFlag(true);
		gameManagement_->SetFailedSelect(1);
	}
	// 子を置いていく
	else if (stageSituation == 3) {
		gameManagement_->SetFailedFlag(true);
		gameManagement_->SetFailedSelect(0);
	}
	// コアラを落とす
	else if (stageSituation == 4) {
		gameManagement_->SetFailedFlag(true);
		gameManagement_->SetFailedSelect(2);
	}

	// この条件式でできない理由 is 何
	if (stageSituation == 0 &&
		!(player->is_rotating() || player->is_moving() || player->is_falling() || child->is_falling()) &&
		moveLogger->can_undo() && Input::IsTriggerKey(KeyID::Z)) {
		undo();
	}
}

void PlayerManager::begin_rendering() {
	player->begin_rendering();
	child->begin_rendering();
	catchEffect_->begin_rendering();
	dustEmitter->begin_rendering();
	iceDustEmitter->begin_rendering();
}

void PlayerManager::draw() const {
	player->draw();
	child->draw();
	catchEffect_->draw();
}

void PlayerManager::draw_particle() {
	dustEmitter->draw();
	iceDustEmitter->draw();
}

void PlayerManager::handle_input() {
	if (player->get_state() != PlayerState::Idle) {
		return;
	}

	Vector3 directions[] = {
		{0.0f, 0.0f, 1.0f},  // 前
		{-1.0f, 0.0f, 0.0f}, // 左
		{0.0f, 0.0f, -1.0f}, // 後ろ
		{1.0f, 0.0f, 0.0f}   // 右
	};

	KeyID keys[] = { KeyID::W, KeyID::A, KeyID::S, KeyID::D };

	for (size_t i = 0; i < 4; ++i) {
		if (Input::GetInstance().IsTriggerKey(keys[i])) {
			//player->set = directions[i];
			Vector3 nextPosition = player->get_translate() + directions[i];

			// 回転中の衝突チェック
			if (mapchipHandler->can_player_rotate(player.get(), child.get(), directions[i])) {
				set_rotate_parameters(directions[i]);
			}
			else {
				set_rotate_failed_parameters(directions[i]);
			}

			// MapchipHandlerに移動可能かを問い合わせ
			if (mapchipHandler->can_player_move_to(player.get(), child.get(), directions[i])) {
				// 成功時のパラメータ設定
				set_move_parameters(directions[i]);
			}
			else {
				set_move_failed_parameters(directions[i]);
			}

			// いろいろと判定が完了したらプレイヤーの状態を回転にする
			player->set_state(PlayerState::Rotating);
			break;
		}
	}
}


#ifdef _DEBUG
void PlayerManager::debug_update() {
	player->debug_update();
	child->debug_update();

	ImGui::Begin("CatchEffect");
	catchEffect_->debug_gui();
	ImGui::End();

	ImGui::Begin("dustEmitter");
	dustEmitter->debug_gui();
	ImGui::End();

	ImGui::Begin("iceDustEmitter");
	iceDustEmitter->debug_gui();
	ImGui::End();
}
#endif

void PlayerManager::particle_update() {
	// パーティクルの制御
// 待機と落下のどちらでもなければよろし
	if (player->get_state() != PlayerState::Idle &&
		player->get_state() != PlayerState::Falling) {
		// その中でも氷の中だったら氷用
		if (player->get_move_type() == MoveType::SlidingOnIce) {
			iceDustEmitter->set_active(true);
		}
		else {
			dustEmitter->set_active(true);
		}
	}
	else {
		dustEmitter->set_active(false);
		iceDustEmitter->set_active(false);
	}
}

void PlayerManager::manage_parent_child_relationship() {
	// 1フレーム前の親子付け
	bool preParent = player->is_parent();

	if (!player->is_parent()) {

		Vector3 playerToChild = player->get_translate() - child->get_translate();
		float distance = playerToChild.length();
		if (distance <= 1.01f) {
			// 子をプレイヤーにくっつける処理
			attach_child_to_player(player.get(), child.get());
			if (child->get_object()->get_animation()->is_end()) {
				//child->get_object()->reset_animated_mesh("ChiledKoala.gltf", "Standby", false);
				child->get_object()->get_animation()->reset_animation("Standby");
				child->get_object()->get_animation()->set_loop(false);
			}

			//前フレ子なし、今フレ子ありならholdを鳴らす
			if (!preParent && player->is_parent()) {
				holdAudio->restart();
			}
		}
	}
	else {
		if (player->is_moving()) {
			return;
		}
		// 子をプレイヤーから切り離す処理
		if (Input::GetInstance().IsTriggerKey(KeyID::Space)) {
			emplace_log(player->get_translate(), player->get_rotation());
			detach_child_from_player(player.get(), child.get());
		}

		//前フレ子あり、今フレ子なしならreleaseを鳴らす
		if (preParent && !player->is_parent()) {
			releaseAudio->restart();
		}
	}
}

void PlayerManager::set_child_rotate() {
	if (player->is_falling() || child->is_falling()) {
		return;
	}

	Vector3 childToPlayer;

	// 子からプレイヤーへの方向ベクトルを計算
	if (childPos != playerPos) {
		childToPlayer = Vector3::Normalize(childPos, playerPos);
	}

	// 子からプレイヤーへの方向ベクトルの長さをチェック
	if (childToPlayer.length() == 0.0f) return;

	// 親オブジェクトの回転を考慮
	if (player->is_parent()) {
		// 親の回転を取得
		Quaternion parentRotation = player->get_rotation();

		// 親の回転を適用して子からプレイヤーへの方向を調整
		childToPlayer = childToPlayer * parentRotation.inverse();
	}

	// 基準方向を設定 (子のデフォルトの前方向、例: Z軸)
	Vector3 defaultForward = { 0.0f, 0.0f, -1.0f };

	// Y軸回転を計算
	Quaternion targetRotation = Quaternion::FromToRotation(defaultForward, childToPlayer);

	// 現在の回転を取得
	Quaternion currentRotation = child->get_rotation();

	// Slerpのロジックを利用して反転防止を実現
	float dot = Vector3::DotProduct(currentRotation.vector(), targetRotation.vector()) + currentRotation.real() * targetRotation.real();
	if (dot < 0.0f) {
		targetRotation = targetRotation * -1.0f;
	}

	// 補間係数 (スムーズに回転する速さを調整)
	float smoothFactor = 0.1f; // 値を小さくするとゆっくり、大きくすると速く回転

	// Slerpを使って補間
	Quaternion smoothedRotation = Quaternion::Slerp(currentRotation, targetRotation, smoothFactor);

	// 補間後の回転を子に設定
	child->set_rotation(targetRotation);
}

void PlayerManager::attach_child_to_player(Player* player, Child* child) {
	// 今フレームで移動していなければ返す
	if (!player->is_moved()) return;

	child->get_object()->reparent(player->get_object());
	child->get_object()->look_at(*player->get_object());
	player->set_parent(true);
	NodeAnimationPlayer* anim = child->get_object()->get_animation();
	anim->reset_animation("Hold");
	anim->set_loop(false);
	anim->restart();
}

void PlayerManager::detach_child_from_player(Player* player, Child* child) {
	if (player->is_moving() && !player->is_on_ice()) {
		return;
	}
	if (player->is_rotating()) {
		return;
	}
	// ペアレントを解消する
	child->get_object()->reparent(nullptr);
	child->get_object()->look_at(*player->get_object());
	// 子供のワールド座標を設定
	child->set_translate({ std::round(childPos.x), std::round(childPos.y), std::round(childPos.z) });
	// 親子付けフラグをオフにする
	player->set_parent(false);
	// アニメーションをセット
	NodeAnimationPlayer* anim = child->get_object()->get_animation();
	if (!child->is_out_ground()) {
		anim->reset_animation("Relese");
		anim->set_loop(false);
		anim->restart();
	}
	else {
		anim->reset_animation("Falling");
		anim->set_loop(true);
		anim->restart();
	}
}

void PlayerManager::emplace_log(const Vector3& playerPosition, const Quaternion& playerRotation) {
	moveLogger->emplace({
		{ playerPosition.x,playerPosition.z, playerRotation },
		{ child->get_translate().x,child->get_translate().z, child->get_rotation() },
		player->is_parent()
		});
}

void PlayerManager::undo() {
	auto popped = moveLogger->pop();

	float childY;
	if (popped.isSticking) {
		childY = 0.0f;
	}
	else {
		childY = 1.0f;
	}

	// Translate設定
	player->on_undo(
		{ popped.player.x, 1.0f, popped.player.y },
		popped.player.rotation,
		popped.isSticking
	);
	child->on_undo(
		{ popped.child.x, childY, popped.child.y },
		player->get_translate()
	);

	AnimatedMeshInstance* childMesh = child->get_object();
	AnimatedMeshInstance* playerMesh = player->get_object();
	NodeAnimationPlayer* childAnimation = childMesh->get_animation();
	NodeAnimationPlayer* playerAnimation = playerMesh->get_animation();

	// 親子付け設定
	// つかみ状態
	if (popped.isSticking) {
		childMesh->reparent(playerMesh, false);
		childMesh->look_at(*playerMesh);
		player->set_parent(true);
	}
	// 非つかみ状態
	else {
		// ペアレントを解消する
		childMesh->reparent(nullptr, false);
		childMesh->look_at(*playerMesh);
		// 補正
		child->set_translate({ std::round(childPos.x), std::round(childPos.y), std::round(childPos.z) });
		// 親子付けフラグをオフにする
		player->set_parent(false);
	}

	player->begin_rendering();
	child->begin_rendering();

	// アニメーション設定
	// くっつき状態
	if (popped.isSticking) {
		// 足元情報
		Vector3 playerWorld = player->get_object()->world_position();
		Vector3 childWorld = child->get_object()->world_position();
		bool isPlayerOnGround = mapchipField_->getElement(playerWorld.x, playerWorld.z) == 1;
		bool isChildOnGround = mapchipField_->getElement(childWorld.x, childWorld.z) == 1;
		// 親
		// 足元が地面
		if (isPlayerOnGround) {
			playerAnimation->reset_animation("Standby");
			playerAnimation->set_loop(true);
		}
		else {
			playerAnimation->reset_animation("Flustered");
			playerAnimation->set_loop(true);
		}
		// 子
		// 足元が地面
		if (isChildOnGround) {
			childAnimation->reset_animation("Hold");
			childAnimation->set_time_force(1000);
			childAnimation->set_loop(false);
		}
		else {
			childAnimation->reset_animation("Flustered");
			childAnimation->set_time_force(1000);
			childAnimation->set_loop(true);
		}
	}
	// 非くっつき状態
	else {
		playerAnimation->reset_animation("Standby");
		playerAnimation->set_loop(true);
		childAnimation->reset_animation("Standby");
		childAnimation->set_loop(true);
	}
}

void PlayerManager::set_move_parameters(const Vector3& direction) {
	Vector3 nextPosition;
	int moveNumOnIce;

	switch (player->get_move_type()) {
	case MoveType::Normal:
		nextPosition = player->get_translate() + direction;
		player->get_target_pos(nextPosition);
		player->set_move_timer(0.0f);
		player->set_move_duration(0.15f);
		player->set_start_position(player->get_translate());
		break;
	case MoveType::SlidingOnIce:
		moveNumOnIce = mapchipHandler->can_player_move_on_ice(player.get(), child.get(), direction);
		nextPosition = player->get_translate() + direction * static_cast<float>(moveNumOnIce);
		player->get_target_pos(nextPosition);
		player->set_move_timer(0.0f);
		player->set_move_duration(0.15f * static_cast<float>(moveNumOnIce));
		player->set_start_position(player->get_translate());
		break;
	}
}

void PlayerManager::set_move_failed_parameters(const Vector3& direction) {
	Vector3 nextPosition;
	switch (player->get_move_type()) {
	case MoveType::MoveOnChild:
		nextPosition = player->get_translate();
		player->get_target_pos(nextPosition);
		player->set_move_timer(0.0f);
		player->set_move_duration(0.01f);
		player->set_start_position(player->get_translate());
		break;
	case MoveType::HitRock:
		//条件を「入力方向に回転する時引っかからない」かつ「進行方向が壁か穴」だと上手く行く
		player->set_wall_target_pos(player->get_translate() + direction * 0.5f);
		player->set_wall_timer(0.0f);
		player->set_wall_duration(0.15f);
		player->set_wall_moving(true);
		player->set_wall_start_pos(player->get_translate());
		break;
	case MoveType::FallIntoHole:
		player->set_wall_target_pos(player->get_translate() + direction * 0.5f);
		player->set_wall_timer(0.0f);
		player->set_wall_duration(0.15f);
		player->set_wall_moving(true);
		player->set_wall_start_pos(player->get_translate());
		break;
	}
}

void PlayerManager::set_rotate_parameters(const Vector3& direction) {
	Vector3 midDir;

	switch (player->get_rotate_type()) {
	case RotateType::Rotate90_Normal:
		player->set_start_rotation(player->get_rotation());
		player->set_target_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, direction));
		player->set_rotate_timer(0.0f);
		player->set_rotate_duration(0.3f);
		player->set_rotating(true);
		break;
	case RotateType::None:
		player->set_start_rotation(player->get_rotation());
		player->set_target_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, direction));
		player->set_rotate_timer(0.0f);
		player->set_rotate_duration(0.01f);
		player->set_rotating(true);
		break;
	case RotateType::Normal:
		player->set_start_rotation(player->get_rotation());
		player->set_target_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, direction));
		player->set_rotate_timer(0.0f);
		player->set_rotate_duration(0.3f);
		player->set_rotating(true);

		if (player->get_how_rotation() == RotationDirection::Left) {
			midDir = GameUtility::rotate_direction_90_left(direction);
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, midDir));
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			midDir = GameUtility::rotate_direction_90_right(direction);
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, midDir));
		}

		break;
	}
}

void PlayerManager::set_rotate_failed_parameters(const Vector3& direction) {
	Vector3 childDirection;
	if (std::round(child->get_translate().x) == 1.0f) {
		childDirection = GameUtility::rotate_direction_90_left(direction);
	}
	else if (std::round(child->get_translate().x) == -1.0f) {
		childDirection = GameUtility::rotate_direction_90_right(direction);
	}
	else {
		childDirection = direction;
	}

	// 現在のプレイヤーの位置
	Vector3 nowPlayerPos = player->get_translate();
	// 今の子供の位置
	Vector3 nowChildPos = nowPlayerPos + child->get_translate() * player->get_rotation();
	// 移動予定の位置
	Vector3 nextChildPos = nowPlayerPos + childDirection;
	// 一回転しない場合の経由点
	Vector3 midChildPos = nowChildPos + childDirection;

	// プレイヤーの向いている方向
	//Vector3 playerDirection = direction;
	// 左方向
	Vector3 leftDirection = GameUtility::rotate_direction_90_left(childDirection);
	// 右方向
	Vector3 rightDirection = GameUtility::rotate_direction_90_right(childDirection);
	// 左斜め方向を計算
	Vector3 leftDiagonalDirection = (childDirection + leftDirection).normalize_safe();
	// 右斜め方向を計算
	Vector3 rightDiagonalDirection = (childDirection + rightDirection).normalize_safe();
	// プレイヤーと子が向かい合ってるときの斜め方向
	Vector3 childDiagonalDirection = (childDirection + player->get_previous_direction()).normalize_safe();

	// 回転角の定義
	const float ANGLE_15 = 15.0f * (3.14f / 180.0f);
	const float ANGLE_40 = 40.0f * (3.14f / 180.0f);
	const float ANGLE_90 = 90.0f * (3.14f / 180.0f);
	const float ANGLE_135 = 135.0f * (3.14f / 180.0f);
	const float ANGLE_175 = 175.0f * (3.14f / 180.0f);

	// クォータニオンの作成
	Quaternion rotate15Left = Quaternion::AngleAxis({ 0.0f, 1.0f, 0.0f }, ANGLE_15);
	Quaternion rotate15Right = Quaternion::AngleAxis({ 0.0f, 1.0f, 0.0f }, -ANGLE_15);
	Quaternion rotate40Left = Quaternion::AngleAxis({ 0.0f, 1.0f, 0.0f }, ANGLE_40);
	Quaternion rotate40Right = Quaternion::AngleAxis({ 0.0f, 1.0f, 0.0f }, -ANGLE_40);
	Quaternion rotate90Left = Quaternion::AngleAxis({ 0.0f, 1.0f, 0.0f }, ANGLE_90);
	Quaternion rotate90Right = Quaternion::AngleAxis({ 0.0f, 1.0f, 0.0f }, -ANGLE_90);
	Quaternion rotate135Left = Quaternion::AngleAxis({ 0.0f, 1.0f, 0.0f }, ANGLE_135);
	Quaternion rotate135Right = Quaternion::AngleAxis({ 0.0f, 1.0f, 0.0f }, -ANGLE_135);
	Quaternion rotate175Left = Quaternion::AngleAxis({ 0.0f, 1.0f, 0.0f }, ANGLE_175);
	Quaternion rotate175Right = Quaternion::AngleAxis({ 0.0f, 1.0f, 0.0f }, -ANGLE_175);

	// 回転失敗時の基本設定
	player->set_start_rotation(player->get_rotation());
	player->set_target_rotation(player->get_rotation());
	player->set_rotate_timer(0.0f);
	player->set_rotate_duration(0.3f);
	player->set_rotating(true);
	player->set_moving(false);

	switch (player->get_rotate_type()) {
	case RotateType::None:
		player->set_target_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, direction));
		player->set_rotate_duration(0.01f);
		break;
	case RotateType::Rotate90_HitObstacleDiagonalFront:
		// 壁にぶつかる移動をセットしておく
		player->set_move_type(MoveType::HitRock);

		player->set_how_rotation(RotationDirection::Reverce);

		if (std::round(child->get_translate().x) == 1.0f) {
			player->set_mid_rotation(rotate15Right * player->get_rotation());
		}
		else if (std::round(child->get_translate().x) == -1.0f) {
			//player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, leftDiagonalDirection));
			player->set_mid_rotation(rotate15Left * player->get_rotation());
		}
		else {
			Vector3 playerForward = Vector3(0.0f, 0.0f, -1.0f) * player->get_rotation();

			if (playerForward.x > 0) {
				if (childDirection.z > 0) {
					player->set_mid_rotation(rotate15Right * player->get_rotation());
				}
				else {
					player->set_mid_rotation(rotate15Left * player->get_rotation());
				}
			}
			else if (playerForward.x < 0) {
				if (childDirection.z > 0) {
					player->set_mid_rotation(rotate15Left * player->get_rotation());
				}
				else {
					player->set_mid_rotation(rotate15Right * player->get_rotation());
				}
			}

			if (playerForward.z > 0) {
				if (childDirection.x > 0) {
					player->set_mid_rotation(rotate15Left * player->get_rotation());
				}
				else {
					player->set_mid_rotation(rotate15Right * player->get_rotation());
				}
			}
			else if (playerForward.z < 0) {
				if (childDirection.x > 0) {
					player->set_mid_rotation(rotate15Right * player->get_rotation());
				}
				else {
					player->set_mid_rotation(rotate15Left * player->get_rotation());
				}
			}
		}
		break;

	case RotateType::Rotate90_HitObstacleNextPosition:
		// 壁にぶつかる移動をセットしておく
		player->set_move_type(MoveType::HitRock);

		player->set_how_rotation(RotationDirection::Reverce);
		if (std::round(child->get_translate().x) == 1.0f) {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, rightDirection));
		}
		else if (std::round(child->get_translate().x) == -1.0f) {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, leftDirection));
		}
		else {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, childDirection));
		}
		break;
	case RotateType::Rotate90_NextPositionIsHole:
		// 穴に落下する時の移動をセットしておく
		player->set_move_type(MoveType::FallIntoHole);

		player->set_how_rotation(RotationDirection::Reverce);
		if (std::round(child->get_translate().x) == 1.0f) {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, rightDirection));
		}
		else if (std::round(child->get_translate().x) == -1.0f) {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, leftDirection));
		}
		else {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, childDirection));
		}
		break;
	case RotateType::HitDiagonalFrontWall:
		// 壁にぶつかる移動をセットしておく
		player->set_move_type(MoveType::HitRock);

		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate15Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate15Right * player->get_rotation());
		}
		break;

	case RotateType::HitSideWall:
		// 壁にぶつかる移動をセットしておく
		player->set_move_type(MoveType::HitRock);

		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate90Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate90Right * player->get_rotation());
		}
		break;
	case RotateType::NextTileIsHole:
		// 穴に落下する時の移動をセットしておく
		player->set_move_type(MoveType::FallIntoHole);

		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate90Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate90Right * player->get_rotation());
		}
		break;
	case RotateType::HitDiagonalBackWall:
		// 壁にぶつかる移動をセットしておく
		player->set_move_type(MoveType::HitRock);

		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate135Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate135Right * player->get_rotation());
		}
		break;
	case RotateType::HitBackWall:
		// 壁にぶつかる移動をセットしておく
		player->set_move_type(MoveType::HitRock);

		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate175Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate175Right * player->get_rotation());
		}
		else {
			player->set_how_rotation(RotationDirection::Reverce);
			player->set_mid_rotation(rotate175Right * player->get_rotation());
		}
		break;
	case RotateType::BackTileIsHole:
		// 穴に落下する時の移動をセットしておく
		player->set_move_type(MoveType::FallIntoHole);

		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate175Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate175Right * player->get_rotation());
		}
		else {
			player->set_how_rotation(RotationDirection::Reverce);
			player->set_mid_rotation(rotate175Right * player->get_rotation());
		}
		break;
	}

	return;
}
