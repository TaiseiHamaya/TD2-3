#include "MapchipHandler.h"
#include "Application/Utility/GameUtility.h"
#include "Application/Character/Player/Player.h"
#include "Application/Character/Child/Child.h"

#include <Engine/Resources/Animation/NodeAnimation/NodeAnimationPlayer.h>

void MapchipHandler::initialize(MapchipField* mapchipField) {
	mapchipField_ = mapchipField;
}

void MapchipHandler::update_player_on_mapchip(Player* player, Child* child) {
	// プレイヤーと子供が落下中かを判定
	check_fall_conditions(player, child);
}

void MapchipHandler::check_fall_conditions(Player* player, Child* child) {
	// 回転中は判定しない
	if (player->is_rotating()) return;
	// 各座標を取得
	Vector3 playerPos = player->get_translate();
	Vector3 childPos = child->get_translate();
	// ペアレントしていたらワールド座標に変換
	if (player->is_parent()) {
		childPos = playerPos + childPos * player->get_rotation();
	}
	// 1フレーム前に穴の上にいるかどうかを取得
	bool prePlayerChip = player->is_out_ground();
	bool preChildChip = child->is_out_ground();
	// 今フレームで穴の上にいるかを判定
	player->set_out_ground(mapchipField_->getElement(std::round(playerPos.x), std::round(playerPos.z)) == 0);
	child->set_out_ground(mapchipField_->getElement(std::round(childPos.x), std::round(childPos.z)) == 0);

	if (prePlayerChip != player->is_out_ground()) {
		if (player->is_out_ground()) {
			player->get_object()->get_animation()->reset_animation("Flustered");
			player->get_object()->get_animation()->set_loop(true);
			player->set_animation_info(PlayerAnimation::Flustered);
		}
		else {
			player->get_object()->get_animation()->reset_animation("Standby");
			player->get_object()->get_animation()->set_loop(true);
			player->set_animation_info(PlayerAnimation::Normal);
		}
	}
	// 子供が穴の上にいるかどうかが前フレームと今フレームで違っていたらアニメーションの切り替え
	if (player->is_parent()) {
		if (preChildChip != child->is_out_ground()) {
			if (child->is_out_ground()) {
				child->get_object()->get_animation()->reset_animation("Flustered");
				child->get_object()->get_animation()->set_loop(true);
				child->set_animation_info(ChildAnimation::Flustered);
			}
			else {
				child->get_object()->get_animation()->reset_animation("Hold");
				child->get_object()->get_animation()->set_time_force(1000);
				child->get_object()->get_animation()->set_loop(false);
				child->set_animation_info(ChildAnimation::Normal);
			}
		}
	}

	// 親と繋がっているか判定
	if (!player->is_parent()) {
		// 繋がっていなければ別々で判定
		if (player->is_out_ground()) {
			player->set_state(PlayerState::Falling);
		}
		player->set_falling(player->is_out_ground());
		child->set_falling(child->is_out_ground());
	}
	else {
		// 繋がっていれば二人同時に空中にいた場合のみ落下
		if (player->is_out_ground() && child->is_out_ground()) {
			player->set_falling(true);
			child->set_falling(true);
		}
	}

}

int MapchipHandler::is_goal_reached(Player* player, Child* child) const {
	// 回転中だったら判定を取らない
	if (player->is_rotating()) {
		return 0;
	}

	// 親の位置を取得
	Vector3 playerPos = player->get_translate();
	int playerChip = mapchipField_->getElement(std::round(playerPos.x), std::round(playerPos.z));

	// ペアレントしていれば
	if (!player->is_parent()) {
		// 子供を置いていった
		if (playerChip == 3) {
			return 3;
		}
	}
	// 以降はペアレントしている場合
	else {
		// 子オブジェクトの現在位置を取得
		Vector3 childPos = child->get_translate();
		childPos = playerPos + childPos * player->get_rotation();
		int childChip = mapchipField_->getElement(std::round(childPos.x), std::round(childPos.z));

		// プレイヤーまたは子供がゴールに到達しているかを確認
		const int GOAL_CHIP = 3; // 3: ゴールとするマップチップのID
		// 親が先にゴールについた
		if (playerChip == GOAL_CHIP) {
			return 1;
		}
		// 子供が先にゴールについた
		if (childChip == GOAL_CHIP) {
			return 2;
		}
	}

	return 0;
}