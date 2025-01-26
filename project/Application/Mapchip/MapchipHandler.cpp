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

bool MapchipHandler::can_player_move_to(Player* player, Child* child, const Vector3& direction) const {
	Vector3 nextPos = player->get_translate() + direction;

	if (!player->is_parent()) {
		if (nextPos == child->get_translate()) {
			return false;
		}
		auto element = mapchipField_->getElement(nextPos.x, nextPos.z);
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

		if (elementNextPlayer == 0 && elementChild == 0) {
			return false;
		}

		if (elementNextPlayer == 2 || elementChild == 2) {
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