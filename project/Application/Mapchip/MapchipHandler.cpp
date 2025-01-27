#include "MapchipHandler.h"
#include "Application/Utility/GameUtility.h"
#include "Application/Character/Player/Player.h"
#include "Application/Character/Child/Child.h"

void MapchipHandler::initialize(MapchipField* mapchipField)
{
	mapchipField_ = mapchipField;
}

void MapchipHandler::update_player_on_mapchip(Player* player, Child* child) {
	// プレイヤーと子供が落下中かを判定
	check_fall_conditions(player, child);
}

bool MapchipHandler::can_player_move_on_ice(Player* player, Child* child, const Vector3& direction) const {
	if (!player->is_on_ice())return false;

	// マップチップを取得
	auto get_next_chip = [&](const Vector3& position) -> int {
		return mapchipField_->getElement(std::round(position.x), std::round(position.z));
		};

	// 子供の位置を取得
	auto calculate_child_position = [&]() -> Vector3 {
		if (!player->is_parent()) return {};
		Vector3 childDirection = direction;
		if (std::round(child->get_translate().x) == 1.0f) {
			childDirection = GameUtility::rotate_direction_90_left(direction);
		}
		else if (std::round(child->get_translate().x) == -1.0f) {
			childDirection = GameUtility::rotate_direction_90_right(direction);
		}
		return player->get_translate() + childDirection + direction * static_cast<float>(player->get_move_num_on_ice());
		};

	// 氷以外のブロックを見つけるまで探索
	while (true) {
		// 現在の移動量を取得
		int moveNum = player->get_move_num_on_ice();
		// 次の位置を取得
		Vector3 nextPos = player->get_translate() + direction * static_cast<float>(moveNum);
		// 次のチップを取得
		int nextChip = get_next_chip(nextPos);

		// 子供がいる場合
		if (player->is_parent()) {
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
				player->set_move_num_on_ice(moveNum);
				break;
			}
			// 壁だったら止まる
			if (nextChip == 2 || nextChildChip == 2) {
				moveNum -= 1;
				player->set_move_num_on_ice(moveNum);
				break;
			}
			// 床どちらも床だったら止まる
			if (nextChip == 1 && nextChildChip == 1) {
				break;
			}
			// 片方が床片方が床だったら
			if (nextChip == 0 && nextChildChip == 1 || nextChildChip == 3) {
				break;
			}
			// 子供が氷で且つプレイヤーが床だったら
			if (nextChildChip == 0 && nextChip == 1 || nextChip == 3) {
				break;
			}
			// 片方が床片方が床だったら
			if (nextChip == 4 && nextChildChip == 1 || nextChildChip == 3) {
				break;
			}
			// 子供が氷で且つプレイヤーが床だったら
			if (nextChildChip == 4 && nextChip == 1 || nextChip == 3) {
				break;
			}
		}
		// parentしていない場合
		else {
			Vector3 nextPosLeftSide = nextPos + GameUtility::rotate_direction_90_left(direction);
			Vector3 nextPosRightSide = nextPos + GameUtility::rotate_direction_90_right(direction);

			if (std::round(nextPos.x) == std::round(child->get_translate().x) &&
				std::round(nextPos.z) == std::round(child->get_translate().z)) {
				moveNum -= 1;
				player->set_move_num_on_ice(moveNum);
				break;
			}
			else if (std::round(nextPosLeftSide.x) == std::round(child->get_translate().x) &&
				std::round(nextPosLeftSide.z) == std::round(child->get_translate().z)) {
				break;
			}
			else if (std::round(nextPosRightSide.x) == std::round(child->get_translate().x) &&
				std::round(nextPosRightSide.z) == std::round(child->get_translate().z)) {
				break;
			}

			// 穴だったら移動しない
			if (nextChip == 0) {
				// そもそも移動できなくする処理
				//moveNum = 1;
				//return false;
				
				// 穴だったら止まる
				moveNum -= 1;
				player->set_move_num_on_ice(moveNum);
				break;
			}
			// 壁だったら止まる
			if (nextChip == 2) {
				moveNum -= 1;
				player->set_move_num_on_ice(moveNum);
				break;
			}
			// 氷意外だったら
			if (nextChip != 4) {
				break;
			}
		}
		moveNum += 1;
		player->set_move_num_on_ice(moveNum);
	}
	return true;
}

bool MapchipHandler::can_player_move_to(Player* player, Child* child, const Vector3& direction) const
{
	Vector3 nextPos = player->get_translate() + direction;

	if (!player->is_parent()) {
		if (nextPos == child->get_translate()) {
			player->set_on_ice(false);
			return false;
		}
		auto element = mapchipField_->getElement(nextPos.x, nextPos.z);
		if (element == 4) {
			player->set_on_ice(true);
		}
		else {
			player->set_on_ice(false);
		}
		return element == 1 || element == 3;
	}
	else {
		Vector3 childDirection{};
		Vector3 nextChildPos{};

		if (std::round(child->get_translate().x) == 1.0f) {
			childDirection = GameUtility::rotate_direction_90_left(direction);
		}
		else if (std::round(child->get_translate().x) == -1.0f) {
			childDirection = GameUtility::rotate_direction_90_right(direction);
		}
		else {
			childDirection = direction;
		}

		nextChildPos = player->get_translate() + childDirection + direction;

		auto elementNextPlayer = mapchipField_->getElement(std::round(nextPos.x), std::round(nextPos.z));
		auto elementChild = mapchipField_->getElement(std::round(nextChildPos.x), std::round(nextChildPos.z));
		// 移動先がどちらも穴だったらfalse
		if (elementNextPlayer == 0 && elementChild == 0) {
			player->set_on_ice(false);
			return false;
		}
		// 移動先のどちらかが壁だったらfalse
		if (elementNextPlayer == 2 || elementChild == 2) {
			player->set_on_ice(false);
			return false;
		}
		// 移動先のどちらかが氷だったらfalse(別の場所で処理をするので)
		if (elementNextPlayer == 4 || elementChild == 4) {
			player->set_on_ice(true);
			return false;
		}

		return true;
	}
}

bool MapchipHandler::can_player_rotate(Player* player, Child* child, const Vector3& direction) const
{
	if (direction == player->get_previous_direction()) {
		return false; // 同じ方向なら回転不要
	}

	if (!player->is_parent()) return true;

	auto check_collision = [&](const Vector3& pos) {
		return mapchipField_->getElement(std::round(pos.x), std::round(pos.z)) == 2;
		};

	Vector3 childDirection;
	if (std::round(child->get_translate().x) == 1.0f) {
		childDirection = GameUtility::rotate_direction_90_left(direction);
	}
	else if (std::round(child->get_translate().x) == -1.0f) {
		childDirection = GameUtility::rotate_direction_90_right(direction);
	}
	else {
		childDirection = direction;
	}

	// 現在のプレイヤーの位置
	Vector3 nowPlayerPos = player->get_translate();
	// 今の子供の位置
	Vector3 nowChildPos = nowPlayerPos + child->get_translate() * player->get_rotation();
	// 移動予定の位置
	Vector3 nextChildPos = nowPlayerPos + childDirection;
	// 一回転しない場合の経由点
	Vector3 midChildPos = nowChildPos + childDirection;

	// 穴や壁のチェック
	if (mapchipField_->getElement(std::round(nowPlayerPos.x), std::round(nowPlayerPos.z)) == 0) {
		if (mapchipField_->getElement(std::round(nextChildPos.x), std::round(nextChildPos.z)) == 0 ||
			mapchipField_->getElement(std::round(midChildPos.x), std::round(midChildPos.z)) == 0) {
			player->set_moving(false);
			return false;
		}
	}

	// 障害物チェック
	if (check_collision(nextChildPos) || check_collision(midChildPos)) {
		player->set_moving(false);
		return false;
	}

	if (std::abs(direction.x + player->get_previous_direction().x) > 0.01f ||
		std::abs(direction.z + player->get_previous_direction().z) > 0.01f) {
		return true; // 180度ではない場合は回転可能
	}

	auto check_side_collisions = [&](const Vector3& primaryDirection, const Vector3& secondaryDirection) {
		Vector3 firstPos = nowPlayerPos + primaryDirection - childDirection;
		Vector3 secondPos = firstPos + secondaryDirection - childDirection;
		Vector3 thirdPos = secondPos + secondaryDirection - childDirection;
		return check_collision(firstPos) || check_collision(secondPos) || check_collision(thirdPos);
		};

	Vector3 leftDirection = GameUtility::rotate_direction_90_left(childDirection);
	Vector3 rightDirection = GameUtility::rotate_direction_90_right(childDirection);

	// 一回転するときは左右どちらも埋まってたら回転不可
	if (check_side_collisions(leftDirection, childDirection)) {
		if (check_side_collisions(rightDirection, childDirection)) {
			player->set_moving(false);
			return false;
		}
	}
	if (check_side_collisions(rightDirection, childDirection)) {
		if (check_side_collisions(leftDirection, childDirection)) {
			player->set_moving(false);
			return false;
		}
	}

	return true; // 衝突がなければ回転を続行
}

void MapchipHandler::setup_rotation_parameters(Player* player, Child* child, const Vector3& direction)
{
	if (!player->is_parent()) return;

	if (std::abs(direction.x + player->get_previous_direction().x) > 0.01f ||
		std::abs(direction.z + player->get_previous_direction().z) > 0.01f) {
		return; // 180度ではない場合は回転可能
	}

	auto check_collision = [&](const Vector3& pos) {
		return mapchipField_->getElement(std::round(pos.x), std::round(pos.z)) == 2;
		};

	Vector3 childDirection;
	if (std::round(child->get_translate().x) == 1.0f) {
		childDirection = GameUtility::rotate_direction_90_left(direction);
	}
	else if (std::round(child->get_translate().x) == -1.0f) {
		childDirection = GameUtility::rotate_direction_90_right(direction);
	}
	else {
		childDirection = direction;
	}

	// 現在のプレイヤーの位置
	Vector3 nowPlayerPos = player->get_translate();
	// 今の子供の位置
	Vector3 nowChildPos = nowPlayerPos + child->get_translate() * player->get_rotation();
	// 移動予定の位置
	Vector3 nextChildPos = nowPlayerPos + childDirection;
	// 一回転しない場合の経由点
	Vector3 midChildPos = nowChildPos + childDirection;

	auto check_side_collisions = [&](const Vector3& primaryDirection, const Vector3& secondaryDirection) {
		Vector3 firstPos = nowPlayerPos + primaryDirection;
		Vector3 secondPos = firstPos + secondaryDirection;
		Vector3 thirdPos = secondPos + secondaryDirection;
		if (childDirection != direction) {
			firstPos -= childDirection;
			secondPos -= childDirection;
			thirdPos -= childDirection;
		}
		return check_collision(firstPos) || check_collision(secondPos) || check_collision(thirdPos);
		};

	Vector3 leftDirection = GameUtility::rotate_direction_90_left(childDirection);
	Vector3 rightDirection = GameUtility::rotate_direction_90_right(childDirection);

	if (check_side_collisions(leftDirection, childDirection)) {
		// 左側が埋まってたら逆回転
		player->set_reverse_rotation(true);
		if (std::round(child->get_translate().x) == 1.0f) {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, -childDirection));
		}
		else if (std::round(child->get_translate().x) == -1.0f) {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, childDirection));
		}
		else {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, rightDirection));
		}
	}

	if (check_side_collisions(rightDirection, childDirection)) {
		// 右側が埋まってたら逆回転
		player->set_reverse_rotation(true);
		if (std::round(child->get_translate().x) == 1.0f) {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, childDirection));
		}
		else if (std::round(child->get_translate().x) == -1.0f) {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, -childDirection));
		}
		else {
			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, leftDirection));
		}
	}
}

void MapchipHandler::check_fall_conditions(Player* player, Child* child) {
	Vector3 playerPos = player->get_translate();
	Vector3 childPos = child->get_translate();

	if (player->is_parent()) {
		childPos = playerPos + childPos * player->get_rotation();
	}

	bool playerFalling = mapchipField_->getElement(std::round(playerPos.x), std::round(playerPos.z)) == 0;
	bool childFalling = mapchipField_->getElement(std::round(childPos.x), std::round(childPos.z)) == 0;

	if (!player->is_parent()) {
		player->set_falling(playerFalling);
		child->set_falling(childFalling);
	}
	else {
		if (playerFalling && childFalling) {
			player->set_falling(true);
			child->set_falling(true);
		}
	}
}

int MapchipHandler::is_goal_reached(Player* player, Child* child) const
{
	if (!player->is_parent()) return 0;

	Vector3 playerPos = player->get_translate();
	int playerChip = mapchipField_->getElement(std::round(playerPos.x), std::round(playerPos.z));

	// 子オブジェクトの現在位置を取得
	Vector3 childPos = child->get_translate();
	childPos = playerPos + childPos * player->get_rotation();
	int childChip = mapchipField_->getElement(std::round(childPos.x), std::round(childPos.z));

	// プレイヤーまたは子供がゴールに到達しているかを確認
	const int GOAL_CHIP = 3; // 3: ゴールとするマップチップのID
	if (playerChip == GOAL_CHIP) {
		return 1;
	}
	if (childChip == GOAL_CHIP) {
		return 2;
	}

	return 0;
}