#include "PlayerManager.h"
#include "Engine/Runtime/Input/Input.h"
#include <cmath>

void PlayerManager::initialize()
{
	player = std::make_unique<Player>();
	player->initialize();
	child = std::make_unique<Child>();
	child->initialize();
}

void PlayerManager::finalize()
{
}

void PlayerManager::begin()
{
}

void PlayerManager::update()
{
	player->update();
	child->update();

    // 親が設定されていない場合のみ処理
    if (!player->get_parent_flag()) {
        // プレイヤーが移動している場合のみ処理
        if (player->get_move_flag()) {
            // 子オブジェクトが隣接しているか判定
            Vector3 directions[] = {
                {-1.0f, 0.0f, 0.0f},   // 右
                {1.0f, 0.0f, 0.0f},  // 左
                {0.0f, 0.0f, -1.0f},   // 前
                {0.0f, 0.0f, 1.0f}   // 後ろ
            };

            Vector3 playerPos = player->get_transform();
            Vector3 childPos = child->get_translate();

            for (const auto& direction : directions) {
                // 親のワールド位置と子の位置をチェック
                Vector3 playerToChild = playerPos - childPos;
                if (ApproximatelyEqual(playerToChild, direction)) {
                    // 子オブジェクトを親に設定
                    child->set_parent(player->get_object());

                    // 回転を考慮してオフセットを計算
                    Quaternion parentRotation = player->get_rotation();
                    Vector3 adjustedOffset = direction * parentRotation;

                    if (adjustedOffset.x == 1.0f || adjustedOffset.x == -1.0f || adjustedOffset.z == 1.0f || adjustedOffset.z == -1.0f) {
                        adjustedOffset *= -1.0f;
                    }

                    // ローカル空間に子を移動
                    child->set_translate(adjustedOffset);

                    // 親子関係フラグを設定
                    player->set_parent_flag(true);
                }
            }
        }
    }
    // 親が設定されている場合は切り離し
    else {
        DetachChildFromPlayer(player.get(), child.get());
    }
}

void PlayerManager::begin_rendering()
{
	player->begin_rendering();
	child->begin_rendering();
}

void PlayerManager::late_update()
{
}

void PlayerManager::draw() const
{
	player->draw();
	child->draw();
}

#ifdef _DEBUG
void PlayerManager::debug_update()
{
	player->debug_update();
}
#endif // _DEBUG

void PlayerManager::DetachChildFromPlayer(Player* player, Child* child)
{
	if (Input::GetInstance().IsTriggerKey(KeyID::Space)) {
		// 子オブジェクトをワールド空間に戻す
		Vector3 worldPosition = player->get_transform() + child->get_translate() * player->get_rotation();
		child->unset_parent();
		child->set_translate(worldPosition);

		// 親子関係フラグを解除
		player->set_parent_flag(false);
	}
}

bool PlayerManager::ApproximatelyEqual(const Vector3& a, const Vector3& b, float epsilon)
{
    return std::fabs(a.x - b.x) < epsilon &&
        std::fabs(a.y - b.y) < epsilon &&
        std::fabs(a.z - b.z) < epsilon;
}
