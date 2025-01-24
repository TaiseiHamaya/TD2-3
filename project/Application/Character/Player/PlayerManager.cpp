#include "PlayerManager.h"
#include "Engine/Runtime/Input/Input.h"
#include <cmath>

void PlayerManager::initialize() {
	player = std::make_unique<Player>();
	player->initialize();

	child = std::make_unique<Child>();
	child->initialize();
	
	if (auto p = dynamic_cast<Player*>(player.get())) {
		p->set_child(child.get());
	}
}

void PlayerManager::finalize() {
	player->finalize();
	child->finalize();
}

void PlayerManager::update() {
	// プレイヤーと子供の位置を計算
	if (auto p = dynamic_cast<Player*>(player.get())) {
		playerPos = p->get_translate_instance();
		childPos = child->get_translate();

		if (p->is_parent()) {
			childPos = playerPos + childPos * p->get_rotation();
		}
	}

	// プレイヤーのマップチップ関連の更新
	if (auto p = dynamic_cast<Player*>(player.get())) {
		p->update(mapchipField_);
	}

	// マップチップ以外の更新
	player->update();
	child->update();

	// くっつく処理と切り離す処理
	if (auto p = dynamic_cast<Player*>(player.get())) {
		if (!p->is_parent()) {
			attach_child_to_player();
		}
		else {
			detach_child_from_player();
		}
	}
	// マップチップの処理の更新
	update_mapchip();
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
	if (auto p = dynamic_cast<Player*>(player.get())) {
		p->debug_update();
	}
	if (auto c = dynamic_cast<Child*>(child.get())) {
		c->debug_update();
	}
}
#endif // _DEBUG

void PlayerManager::update_mapchip() {

	check_fall_conditions();
}

void PlayerManager::check_fall_conditions()
{
	if (auto p = dynamic_cast<Player*>(player.get())) {
		// 落下するかどうかを計算
		bool playerFalling = mapchipField_->getElement(playerPos.x, playerPos.z) == 0;
		bool childFalling = mapchipField_->getElement(childPos.x, childPos.z) == 0;

		// 親がいない場合は個々で落下
		if (!p->is_parent()) {
			p->set_falling(playerFalling);
			if (auto c = dynamic_cast<Child*>(child.get())) {
				c->set_falling(childFalling);
			}
		}
		// 親がいた場合は二人とも同時に取る
		else {
			if (playerFalling && childFalling) {
				p->set_falling(true);
				if (auto c = dynamic_cast<Child*>(child.get())) {
					c->set_falling(true);
				}
			}
		}
	}
}

void PlayerManager::attach_child_to_player() {
	if (auto p = dynamic_cast<Player*>(player.get())) {
		if (p->is_moving()) {
			Vector3 directions[] = {
				{1.0f, 0.0f, 0.0f},   // 右
				{-1.0f, 0.0f, 0.0f},    // 左
				{0.0f, 0.0f, 1.0f},   // 前
				{0.0f, 0.0f, -1.0f}     // 後ろ
			};

			for (const auto& direction : directions) {
				// プレイヤーと子供の距離を計算
				Vector3 playerToChild = p->get_translate_instance() - child->get_translate();
				// 近かったら結合
				if (approximately_equal(playerToChild, direction)) {
					child->get_object()->reparent(p->get_object());
					// プレイヤーの回転を考慮してオフセットを生成
					Quaternion parentRotation = p->get_rotation();
					Vector3 adjustedOffset = direction * parentRotation;
					// ごり押しで微調整
					if (std::abs(adjustedOffset.x) == 1.0f || std::abs(adjustedOffset.z) == 1.0f) {
						adjustedOffset *= -1.0f;
					}
					// 子供のローカル座標を設定
					child->set_translate(adjustedOffset);
					p->set_parent(true);
					break;
				}
			}
		}
	}
}

void PlayerManager::detach_child_from_player() {
	if (Input::GetInstance().IsTriggerKey(KeyID::Space)) {
		if (auto p = dynamic_cast<Player*>(player.get())) {
			// ペアレントを解消する
			child->get_object()->reparent(nullptr);
			// 子供のワールド座標を設定
			child->set_translate(childPos);
			// 親子付けフラグをオフにする
			p->set_parent(false);
		}
	}
}

bool PlayerManager::approximately_equal(const Vector3& a, const Vector3& b, float epsilon) {
	return std::fabs(a.x - b.x) < epsilon &&
		std::fabs(a.y - b.y) < epsilon &&
		std::fabs(a.z - b.z) < epsilon;
}

void PlayerManager::check_child_collision()
{
	// 回転中に子供が壁にぶつかる場合
	if (auto p = dynamic_cast<Player*>(player.get())) {
		Vector3 childNextPosition = childPos + p->get_direction() * deltaTime;
		if (mapchipField_->getElement(childNextPosition.x, childNextPosition.z) == 2) {
			p->cancel_rotation(true); // 回転を中断する処理
		}
	}
}
