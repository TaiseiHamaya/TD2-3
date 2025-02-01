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

	// プレイヤーと子供の位置を計算
	playerPos = player->get_translate();
	childPos = child->get_object()->world_position();

	isParent = player->is_parent();//プレイヤーのくっつき状態のフラグを取得
	// マップチップ関連の更新
	mapchipHandler->update_player_on_mapchip(player.get(), child.get());

	// 状態の更新
	player->update();
	child->update();

	if (player->is_stack_movement()) {
		emplace_log(player->move_start_position(), player->start_rotation());
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
}

void PlayerManager::draw() const {
	player->draw();
	child->draw();
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

	bool isPlayerOnGround = mapchipField_->getElement(popped.player.x, popped.player.y) == 1;
	bool isChildOnGround = mapchipField_->getElement(popped.child.x, popped.child.y) == 1;

	float childY;
	if (popped.isSticking) {
		childY = 0.0f;
	}
	else {
		childY = 1.0f;
	}

	player->on_undo(
		{ popped.player.x, 1.0f, popped.player.y },
		popped.player.rotation,
		popped.isSticking,
		isPlayerOnGround
	);

	child->on_undo(
		{ popped.child.x, childY, popped.child.y },
		player->get_translate(),
		popped.isSticking,
		isChildOnGround
	);

	if (popped.isSticking) {
		child->get_object()->reparent(player->get_object(), false);
		child->get_object()->look_at(*player->get_object());
		player->set_parent(true);
		child->get_object()->get_animation()->reset_animation("Hold");
		child->get_object()->get_animation()->set_loop(false);
	}
	else {
		// ペアレントを解消する
		child->get_object()->reparent(nullptr, false);
		child->get_object()->look_at(*player->get_object());
		// 子供のワールド座標を設定
		//child->set_translate({ std::round(childPos.x), std::round(childPos.y), std::round(childPos.z) });
		// 親子付けフラグをオフにする
		player->set_parent(false);
		// アニメーションをセット
		child->get_object()->get_animation()->reset_animation("Standby");
		child->get_object()->get_animation()->set_loop(false);
	}
}
