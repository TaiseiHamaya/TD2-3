#include "PlayerManager.h"

#include <cmath>

#include "Engine/Resources/Animation/NodeAnimation/NodeAnimationPlayer.h"
#include "Engine/Runtime/Input/Input.h"

#include <Application/Utility/GameUtility.h>

void PlayerManager::initialize(Reference<const LevelLoader> level, MapchipField* mapchipField) {
	mapchipField_ = mapchipField;

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

	moveLogger = std::make_unique<MoveLogger>();
	moveLogger->initialize();
}

void PlayerManager::finalize() {
	player->finalize();
	child->finalize();
}

void PlayerManager::update() {
	// クリアか失敗のフラグが立ってたら早期リターン
	if (stageSituation != 0) return;
	isStackMovement = false;

	// プレイヤーと子供の位置を計算
	playerPos = player->get_translate();
	childPos = child->get_object()->world_position();

	isParent = player->is_parent();//プレイヤーのくっつき状態のフラグを取得
	// マップチップ関連の更新
	mapchipHandler->update_player_on_mapchip(player.get(), child.get());
	// 入力処理を受け付ける
	handle_input();

	// 状態の更新
	player->update();
	child->update();

	if (player->is_stack_movement()) {
		emplace_log();
	}

	// 親子関係の管理
	manage_parent_child_relationship();

	set_child_rotate();

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

	if (stageSituation == 0 && isStackMovement) {
		emplace_log();
	}

	if (stageSituation == 0 && moveLogger->can_undo() && Input::IsTriggerKey(KeyID::Z)) {
		undo();
	}
}

void PlayerManager::begin_rendering() {
	player->begin_rendering();
	child->begin_rendering();
}

void PlayerManager::draw() const {
	player->draw();
	child->draw();
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

			// MapchipHandlerに移動可能かを問い合わせ
			if (mapchipHandler->can_player_move_to(player.get(), child.get(), directions[i])) {
				// 成功時のパラメータ設定
				set_move_parameters(directions[i]);
			}
			else {
				set_move_failed_parameters(directions[i]);
			}

			// 回転中の衝突チェック
			if (mapchipHandler->can_player_rotate(player.get(), child.get(), directions[i])) {
				set_rotate_parameters(directions[i]);
			}
			else {
				set_rotate_failed_parameters(directions[i]);
			}
			//進行先が壁か穴かを判定
			// 回転途中に壁がある時に上手く回転しないバグによってこっちもバグってる
			//条件を「入力方向に回転する時引っかからない」かつ「進行方向が壁か穴」だと上手く行く
			//if (mapchipHandler->player_move_to_wall_or_holl(player.get(), child.get(), directions[i])) {
			//	wallTargetPosition = get_translate() + direction * 0.5f;
			//	wallMoveTimer = 0.0f;
			//	wallMoveDuration = 0.15f;
			//	isWallMoveing = true;
			//	wallStartPosition = object_->get_transform().get_translate();
			//}
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
}
#endif

void PlayerManager::manage_parent_child_relationship() {
	// 1フレーム前の親子付け
	bool preParent = player->is_parent();

	if (!player->is_parent()) {
		// 子をプレイヤーにくっつける処理
		attach_child_to_player(player.get(), child.get());
		if (child->get_object()->get_animation()->is_end()) {
			child->get_object()->reset_animated_mesh("ChiledKoala.gltf", "Standby", false);
		}
	}
	else {
		// 子をプレイヤーから切り離す処理
		if (Input::GetInstance().IsTriggerKey(KeyID::Space)) {
			isStackMovement = true;
			detach_child_from_player(player.get(), child.get(), true);
		}
	}
}

void PlayerManager::set_child_rotate() {
	if (player->is_falling() || child->is_falling()) {
		return;
	}

	// 子からプレイヤーへの方向ベクトルを計算
	Vector3 childToPlayer = Vector3::Normalize(childPos, playerPos);

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

	Vector3 playerToChild = player->get_translate() - child->get_translate();
	if (playerToChild.length() > 1.0f) {
		return;
	}

	child->get_object()->reparent(player->get_object());
	child->get_object()->look_at(*player->get_object());
	player->set_parent(true);
	child->get_object()->reset_animated_mesh("ChiledKoala.gltf", "Hold", false);
}

void PlayerManager::detach_child_from_player(Player* player, Child* child, bool isDetachAnimation) {
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
	//child->set_translate({ std::round(childPos.x), std::round(childPos.y), std::round(childPos.z) });
	// 親子付けフラグをオフにする
	player->set_parent(false);
	// アニメーションをセット
	if (!child->is_out_ground()) {
		if (isDetachAnimation) {
			child->get_object()->reset_animated_mesh("ChiledKoala.gltf", "Relese", false);
		}
		else {
			child->get_object()->reset_animated_mesh("ChiledKoala.gltf", "Standby", false);
		}
	}
	else {
		child->get_object()->reset_animated_mesh("ChiledKoala.gltf", "Falling", true);
	}
}

void PlayerManager::emplace_log() {
	Vector3 playerPosition = player->move_start_position();
	moveLogger->emplace({
		{ playerPosition.x,playerPosition.z, player->start_rotation() },
		{ child->get_translate().x,child->get_translate().z, child->get_rotation() },
		player->is_parent()
		});
}

void PlayerManager::undo() {
	auto popped = moveLogger->pop();

	bool isPlayerOnGround = mapchipField_->getElement(popped.player.x, popped.player.y) == 1;
	bool isChildOnGround = mapchipField_->getElement(popped.child.x, popped.child.y) == 1;

	float childY;
	if (popped.isSticking) {
		childY = 0.0f;
	}
	else {
		childY = 1.0f;
	}

	child->on_undo(
		{ popped.child.x, childY, popped.child.y },
		player->get_translate(),
		popped.isSticking,
		isChildOnGround
	);

	player->on_undo(
		{ popped.player.x, 1.0f, popped.player.y },
		popped.player.rotation,
		popped.isSticking,
		isPlayerOnGround
	);

	if (popped.isSticking) {
		attach_child_to_player(player.get(), child.get());
	}
	else {
		detach_child_from_player(player.get(), child.get(), false);
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
		nextPosition = player->get_translate();
		player->get_target_pos(nextPosition);
		player->set_move_timer(0.0f);
		player->set_move_duration(0.01f);
		player->set_start_position(player->get_translate());
		break;
	case MoveType::FallIntoHole:
		nextPosition = player->get_translate();
		player->get_target_pos(nextPosition);
		player->set_move_timer(0.0f);
		player->set_move_duration(0.01f);
		player->set_start_position(player->get_translate());
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
	Vector3 leftDiagonalDirection = (childDirection + leftDirection).normalize();
	// 右斜め方向を計算
	Vector3 rightDiagonalDirection = (childDirection + rightDirection).normalize();
	// プレイヤーと子が向かい合ってるときの斜め方向
	Vector3 childDiagonalDirection = (childDirection + player->get_previous_direction()).normalize();

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
		player->set_how_rotation(RotationDirection::Reverce);

		if (std::round(child->get_translate().x) == 1.0f) {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, rightDiagonalDirection));
		}
		else if (std::round(child->get_translate().x) == -1.0f) {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, leftDiagonalDirection));
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
	case RotateType::Rotate90_NextPositionIsHole:
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
		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate15Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate15Right * player->get_rotation());
		}
		break;

	case RotateType::HitSideWall:
		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate90Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate90Right * player->get_rotation());
		}
		break;
	case RotateType::NextTileIsHole:
		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate90Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate90Right * player->get_rotation());
		}
		break;
	case RotateType::HitDiagonalBackWall:
		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate135Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate135Right * player->get_rotation());
		}
		break;
	case RotateType::HitBackWall:
		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate175Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate175Right * player->get_rotation());
		}
		break;
	case RotateType::BackTileIsHole:
		if (player->get_how_rotation() == RotationDirection::Left) {
			player->set_mid_rotation(rotate175Left * player->get_rotation());
		}
		else if (player->get_how_rotation() == RotationDirection::Right) {
			player->set_mid_rotation(rotate175Right * player->get_rotation());
		}
		break;
	}

	return;
}

