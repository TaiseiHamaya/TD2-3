#include "PlayerStateMoveFailed.h"
#include <Application/Character/Player/Player.h>

void PlayerStateMoveFailed::Enter(Player& player) {
	wallMoveTimer = 0.0f;
}

void PlayerStateMoveFailed::Update(Player& player) {
	//if (!isWallMoveing) { return; }//壁に向かって移動していない時は早期リターン
	wallMoveTimer += WorldClock::DeltaSeconds();

	Vector3 newPos = { 0,1,0 };

	//時間の半分はスタート位置から壁に向かって移動
	//多分正確に半分を計測できる訳じゃないから微妙に戻りすぎる説ある
	if (wallMoveTimer <= player.get_move_duration() * 0.5f) {
		newPos = Vector3::Lerp(player.get_wall_start_pos(), player.get_wall_target_pos(), wallMoveTimer / (player.get_wall_duration() * 0.5f));
	}
	else {
		newPos = Vector3::Lerp(player.get_wall_target_pos(), player.get_wall_start_pos(), wallMoveTimer / (player.get_wall_duration() * 0.5f));
	}

	if (wallMoveTimer >= player.get_wall_duration() * 0.5f) {
		//if (!unmovableFlag) {
		//	unmovable->restart();
		//	unmovableFlag = true;
		//}
	}

	if (wallMoveTimer >= player.get_wall_duration()) {
		player.change_state("Idle");
		return;
	}
	player.set_translate(newPos);
}

void PlayerStateMoveFailed::Exit(Player& player) {
	wallMoveTimer = player.get_wall_duration();
	player.set_wall_moving(false);
	player.set_translate(player.get_wall_start_pos());

	player.set_state(PlayerState::Idle);
	if (player.get_move_type() == MoveType::MoveOnChild) {
		player.set_move(true);
	}

	player.set_move_type(MoveType::Normal);
	//unmovableFlag = false;
}
