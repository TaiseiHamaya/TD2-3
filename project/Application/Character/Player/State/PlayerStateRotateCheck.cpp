#include "PlayerStateRotateCheck.h"
#include "Application/Character/Player/Player.h"
#include <Application/Utility/GameUtility.h>
#include <Application/Character/Player/Strategy/Rotate90Strategy.h>
#include <Application/Character/Player/Strategy/Rotate180Strategy.h>

void PlayerStateRotateCheck::Enter(Player& player) {
	const Vector3& moveDir = player.get_move_direction();

	// 子供の方向からchildDirectionを決定
	if (std::round(player.get_child()->get_translate().x) == 1.0f) {
		childDirection = GameUtility::rotate_direction_90_left(moveDir);
	}
	else if (std::round(player.get_child()->get_translate().x) == -1.0f) {
		childDirection = GameUtility::rotate_direction_90_right(moveDir);
	}
	else {
		childDirection = moveDir;
	}

	// 回転の種類を決定
	const Quaternion nowRot = player.get_rotation();
	const Quaternion nextRot = Quaternion::FromToRotation({ 0, 0, -1 }, moveDir);

	if (GameUtility::IsRotation90Degrees(nowRot, nextRot)) {
		strategy_ = std::make_unique<Rotate90Strategy>();
	}
	else if (GameUtility::IsRotation180Degrees(nowRot, nextRot)){
		strategy_ = std::make_unique<Rotate180Strategy>();
	}
	else {
		player.set_start_rotation(player.get_rotation());
		player.change_state("MoveCheck");
	}
}

void PlayerStateRotateCheck::Update(Player& player) {
	if (!strategy_) return;

	if (!strategy_->CanRotate(player, childDirection)) {
		// 失敗
		player.change_state("RotateFailed");
	}
}


void PlayerStateRotateCheck::Exit(Player& player) {
	// ここで回転に必要なパラメータの設定

}
