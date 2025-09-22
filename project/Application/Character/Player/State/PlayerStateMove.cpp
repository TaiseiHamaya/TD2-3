#include "PlayerStateMove.h"
#include <Engine/Runtime/WorldClock/WorldClock.h>
#include <Application/Character/Player/Player.h>

void PlayerStateMove::Enter(Player& player) {
	moveTimer = 0.0f;
	player.set_state(PlayerState::Moving);
}

void PlayerStateMove::Update(Player& player) {
	//if (player.get_move_type() != MoveType::Normal &&
	//player.get_move_type() != MoveType::ParentCarriesChild &&
	//player.get_move_type() != MoveType::ChildCarriesParent &&
	//player.get_move_type() != MoveType::SlidingOnIce) {
	//	player.set_state(PlayerState::MoveFailed);
	//	return;
	//}

	//isMoving = true;

	//移動時の音。moveTimerが加算される前に処理して一回だけ鳴らす
	if (moveTimer <= 0) {
		//氷と通常床で音を変える
		if (player.get_move_type() == MoveType::SlidingOnIce) {
			//iceMove->restart();
		}
		else {
			//moveAudio->restart();
		}
	}
	// 移動中なら補間処理を実行
	moveTimer += WorldClock::DeltaSeconds();

	// 現在の位置を補間
	Vector3 position = Vector3::Lerp(player.get_translate(), player.get_target_pos(), moveTimer / player.get_move_duration());
	player.set_translate(position);

	if (player.get_move_type() == MoveType::SlidingOnIce) {
		if (moveTimer >= player.get_move_duration() - (player.get_move_duration() * 0.30f)) {
			player.change_state("Idle");
			return;
		}
	}
	else {
		if (moveTimer >= player.get_move_duration()) {
			player.change_state("Idle");
			player.set_translate(player.get_target_pos());
			return;
		}
	}
}

void PlayerStateMove::Exit(Player& player) {
	// 移動完了
	moveTimer = player.get_move_duration();
	player.set_moving(false);
	player.set_move(true);
	//isMoved = true;
	player.set_on_ice(false);
	player.set_stack_movement(true);


}
