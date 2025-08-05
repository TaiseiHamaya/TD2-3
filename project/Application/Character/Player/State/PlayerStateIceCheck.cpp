#include "PlayerStateIceCheck.h"
#include <Library/Math/Vector3.h>
#include <Application/Character/Player/Player.h>
#include <Application/Utility/GameUtility.h>

void PlayerStateIceCheck::Enter(Player& player)
{

	// 移動量を宣言
	moveNum = 1;
}

void PlayerStateIceCheck::Update(Player& player)
{
	// マップチップを取得
	auto get_next_chip = [&](const Vector3& position) -> int {
		return player.get_mapchip()->getElement(std::round(position.x), std::round(position.z));
	};
	


	// 子供の位置を取得
	auto calculate_child_position = [&]() -> Vector3 {
		if (!player.is_parent()) return {};
		Vector3 childDirection = player.get_move_direction();
		if (std::round(player.get_child()->get_translate().x) == 1.0f) {
			childDirection = GameUtility::rotate_direction_90_left(player.get_move_direction());
		}
		else if (std::round(player.get_child()->get_translate().x) == -1.0f) {
			childDirection = GameUtility::rotate_direction_90_right(player.get_move_direction());
		}
		return player.get_translate() + childDirection + player.get_move_direction() * static_cast<float>(moveNum);
	};

	// 氷以外のブロックを見つけるまで探索
	while (true) {
		// 現在の移動量を取得
		//int moveNum = player->get_move_num_on_ice();
		// 次の位置を取得
		Vector3 nextPos = player.get_translate() + player.get_move_direction() * static_cast<float>(moveNum);
		// 次のチップを取得
		int nextChip = get_next_chip(nextPos);

		// 子供がいる場合
		if (player.is_parent()) {
			// 子供の位置の計算
			Vector3 nextChildPos = calculate_child_position();
			int nextChildChip = get_next_chip(nextChildPos);

			// 子と親どちらも穴だったら移動しない
			if (nextChip == 0 && nextChildChip == 0) {
				// そもそも移動できなくする処理
				//moveNum = 1;
				//return false; 

				// 穴だったら止まる
				moveNum -= 1;
				//player->set_move_num_on_ice(moveNum);
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}
			// 壁だったら止まる
			if (nextChip == 2 || nextChildChip == 2) {
				moveNum -= 1;
				player.set_move_num_on_ice(moveNum);
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}
			// 床どちらも床だったら止まる
			if (nextChip == 1 && nextChildChip == 1) {
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}
			// 片方が床片方が床だったら
			if (nextChip == 0 && nextChildChip == 1 || nextChildChip == 3) {
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}
			// 子供が氷で且つプレイヤーが床だったら
			if (nextChildChip == 0 && nextChip == 1 || nextChip == 3) {
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}
			// 片方が床片方が床だったら
			if (nextChip == 4 && nextChildChip == 1 || nextChildChip == 3) {
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}
			// 子供が氷で且つプレイヤーが床だったら
			if (nextChildChip == 4 && nextChip == 1 || nextChip == 3) {
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}
		}
		// parentしていない場合
		else {
			Vector3 nextPosLeftSide = nextPos + GameUtility::rotate_direction_90_left(player.get_move_direction());
			Vector3 nextPosRightSide = nextPos + GameUtility::rotate_direction_90_right(player.get_move_direction());

			// 穴だったら移動しない
			if (nextChip == 0) {
				// そもそも移動できなくする処理
				//moveNum = 1;
				//return false;

				// 穴だったら止まる
				moveNum -= 1;
				//player->set_move_num_on_ice(moveNum);
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}
			// 壁だったら止まる
			if (nextChip == 2) {
				moveNum -= 1;
				//player->set_move_num_on_ice(moveNum);
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}

			// 移動中に子供とぶつかるか判定
			if (std::round(nextPos.x) == std::round(player.get_child()->get_translate().x) &&
				std::round(nextPos.z) == std::round(player.get_child()->get_translate().z)) {
				// 正面衝突する場合だったら1ます後ろにずらす
				moveNum -= 1;
				//player->set_move_num_on_ice(moveNum);
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}
			//　それ以外ならそのまま抜ける
			else if (std::round(nextPosLeftSide.x) == std::round(player.get_child()->get_translate().x) &&
				std::round(nextPosLeftSide.z) == std::round(player.get_child()->get_translate().z)) {
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}
			else if (std::round(nextPosRightSide.x) == std::round(player.get_child()->get_translate().x) &&
				std::round(nextPosRightSide.z) == std::round(player.get_child()->get_translate().z)) {
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}

			// 床だったらそのまま出る
			if (nextChip == 0) {
				player.set_move_num_on_ice(moveNum);
				player.change_state("Move");
				return;
			}


		}
		moveNum += 1;
		player.set_target_pos(player.get_translate() + player.get_move_direction());
		player.set_move_timer(0.0f);
		player.set_move_duration(0.1f * moveNum);
	}
}

void PlayerStateIceCheck::Exit(Player& player)
{
	player.set_target_pos(player.get_translate() + player.get_move_direction() * static_cast<float>(moveNum));
	player.set_move_timer(0.0f);
	player.set_move_duration(0.1f * moveNum);

}
