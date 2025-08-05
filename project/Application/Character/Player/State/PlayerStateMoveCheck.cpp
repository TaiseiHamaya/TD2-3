#include "PlayerStateMoveCheck.h"
#include "Application/Character/Player/Player.h"
#include <Application/Utility/GameUtility.h>

void PlayerStateMoveCheck::Enter(Player& player) {
}

void PlayerStateMoveCheck::Update(Player& player) {
	// 既に回転のほうが失敗していたら回転も失敗
	if (player.get_move_type() == MoveType::FallIntoHole ||
		player.get_move_type() == MoveType::HitRock) {
		player.change_state("MoveFailed");
		return;
	}

	// 落下中だったら移動不可
	if (player.is_falling()) {
		player.change_state("MoveFailed");
		return;
	}
	// プレイヤーの移動予定地
	Vector3 nextPos = player.get_translate() + player.get_move_direction();
	// 親子付けしていない場合
	if (!player.is_parent()) {
		// 移動後の座標が子供と重なってたら移動不可
		if (GameUtility::approximately_equal(nextPos, player.get_child()->get_translate(), 0.1f)) {
			player.set_move_type(MoveType::MoveOnChild);

			player.set_wall_target_pos(player.get_translate());
			player.set_wall_timer(0.0f);
			player.set_wall_duration(0.1f);
			player.set_wall_start_pos(player.get_translate());

			player.change_state("MoveFailed");
			return;
		}
		// 移動先のマップチップ番号を取得
		auto element = player.get_mapchip()->getElement(nextPos.x, nextPos.z);
		// 移動先が氷だったら氷上にいるフラグをオン
		if (element == 4) {
			player.set_move_type(MoveType::SlidingOnIce);
			player.change_state("IceCheck");
			return;
		}

		player.set_wall_target_pos(player.get_translate() + player.get_move_direction() * 0.5f);
		player.set_wall_timer(0.0f);
		player.set_wall_duration(0.1f);
		player.set_wall_moving(true);
		player.set_wall_start_pos(player.get_translate());

		// もし穴だったら
		if (element == 0) {
			player.set_move_type(MoveType::FallIntoHole);
			player.change_state("MoveFailed");
			return;
		}
		// もし壁だったら
		if (element == 2) {
			player.set_move_type(MoveType::HitRock);
			player.change_state("MoveFailed");
			return;
		}

		player.set_target_pos(player.get_translate() + player.get_move_direction());
		player.set_move_timer(0.0f);
		player.set_move_duration(0.1f);
		player.set_start_position(player.get_translate());

		if (element == 1 || element == 3) {
			player.set_move_type(MoveType::Normal);
			player.change_state("Move");
			return;
		}
		// ここまで来たら普通に通る
		player.change_state("Move");
		return;
	}
	else {
		Vector3 childDirection;
			if (std::round(player.get_child()->get_translate().x) == 1.0f) {
				childDirection = GameUtility::rotate_direction_90_left(player.get_move_direction());
			}
			else if (std::round(player.get_child()->get_translate().x) == -1.0f) {
				childDirection = GameUtility::rotate_direction_90_right(player.get_move_direction());
			}
			else {
				childDirection = player.get_move_direction();
			}

			Vector3 nextChildPos = player.get_translate() + childDirection + player.get_move_direction();

		auto elementNextPlayer = player.get_mapchip()->getElement(std::round(nextPos.x), std::round(nextPos.z));
		auto elementChild = player.get_mapchip()->getElement(std::round(nextChildPos.x), std::round(nextChildPos.z));

		// 移動失敗時のパラメータをあらかじめセットしておく
		player.set_wall_target_pos(player.get_translate() + player.get_move_direction() * 0.5f);
		player.set_wall_timer(0.0f);
		player.set_wall_duration(0.1f);
		player.set_wall_moving(true);
		player.set_wall_start_pos(player.get_translate());

		// 移動先がどちらも穴だったらfalse
		if (elementNextPlayer == 0 && elementChild == 0) {
			player.set_move_type(MoveType::FallIntoHole);
			player.change_state("MoveFailed");
			return;
		}
		// 移動先のどちらかが壁だったらfalse
		if (elementNextPlayer == 2 || elementChild == 2) {
			player.set_move_type(MoveType::HitRock);
			player.change_state("MoveFailed");
			return;
		}


		// 移動先がどちらも氷またはどちらかが氷どちらかが穴の場合
		if ((elementNextPlayer == 4 || elementNextPlayer == 0) && (elementChild == 4 || elementChild == 0)) {
			player.set_move_type(MoveType::SlidingOnIce);
			player.change_state("IceCheck");
			return;
		}

		// ここまで来たら移動確定なのでパラメータを渡しておく
		player.set_target_pos(player.get_translate() + player.get_move_direction());
		player.set_move_timer(0.0f);
		player.set_move_duration(0.1f);
		player.set_start_position(player.get_translate());

		// 特に何もなければ移動可能
		player.set_move_type(MoveType::Normal);
		player.change_state("Move");
		return;
	}
}

void PlayerStateMoveCheck::Exit(Player& player) {
	player.set_target_pos(player.get_translate() + player.get_move_direction());
	player.set_move_timer(0.0f);
	player.set_move_duration(0.1f);
	player.set_start_position(player.get_translate());
}
