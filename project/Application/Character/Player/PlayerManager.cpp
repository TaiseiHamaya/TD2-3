#include "PlayerManager.h"
#include "Engine/Runtime/Input/Input.h"
#include <cmath>
#include <Application/Utility/GameUtility.h>
#include "Library/Math/Matrix4x4.h"

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
}

void PlayerManager::finalize() {
	player->finalize();
	child->finalize();
}

void PlayerManager::update() {
	// クリアか失敗のフラグが立ってたら早期リターン
	if (stageSituation != 0) return;

	// プレイヤーと子供の位置を計算
	playerPos = player->get_translate();
	childPos = child->get_translate();

	if (player->is_parent()) {
		childPos = playerPos + childPos * player->get_rotation();
	}

	// マップチップ関連の更新
	mapchipHandler->update_player_on_mapchip(player.get(), child.get());

	// 状態の更新
	player->update();
	child->update();

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
}

void PlayerManager::begin_rendering()
{
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

void PlayerManager::manage_parent_child_relationship()
{
	if (!player->is_parent()) {
		// 子をプレイヤーにくっつける処理
		attach_child_to_player(player.get(), child.get());
	}
	else {
		// 子をプレイヤーから切り離す処理
		detach_child_from_player(player.get(), child.get());
	}
}

void PlayerManager::set_child_rotate()
{
	if (player->is_falling() || child->is_falling()) {
		return;
	}

	// 子からプレイヤーへの方向ベクトルを計算
	 Vector3 childToPlayer = Vector3::Normalize(childPos, playerPos);
	
	 // 子からプレイヤーへの方向ベクトルの長さをチェック
	 if (childToPlayer.length() == 0.0f) return;

	 // 親オブジェクトの回転を考慮
	 if (player->is_parent())
	 {
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

void PlayerManager::attach_child_to_player(Player* player, Child* child)
{
	if (!player->is_moved()) return;

	Vector3 directions[] = {
		{1.0f, 0.0f, 0.0f},   // 右
		{-1.0f, 0.0f, 0.0f},  // 左
		{0.0f, 0.0f, 1.0f},   // 前
		{0.0f, 0.0f, -1.0f}   // 後ろ
	};


	for (const auto& direction : directions) {
		Vector3 playerToChild = player->get_translate() - child->get_translate();
		if (GameUtility::approximately_equal(playerToChild, direction)) {
			child->get_object()->reparent(player->get_object());
			player->set_parent(true);
			// プレイヤーの回転を考慮してオフセットを生成
			Quaternion parentRotation = player->get_rotation();
			Vector3 adjustedOffset = direction * parentRotation;
			// ごり押しで微調整
			if (std::abs(adjustedOffset.x) == 1.0f || std::abs(adjustedOffset.z) == 1.0f) {
				adjustedOffset *= -1.0f;
			}
			// 子供のローカル座標を設定
			child->set_translate(adjustedOffset);
			break;
		}
	}
}

void PlayerManager::detach_child_from_player(Player* player, Child* child)
{
	if (Input::GetInstance().IsTriggerKey(KeyID::Space)) {
		if (player->is_moving() && !player->is_on_ice()) {
			return;
		}
		if (player->is_rotating()) {
			return;
		}
		// ペアレントを解消する
		child->get_object()->reparent(nullptr);
		// 子供のワールド座標を設定
		child->set_translate({ std::round(childPos.x), std::round(childPos.y), std::round(childPos.z) });
		// 親子付けフラグをオフにする
		player->set_parent(false);
	}
}