#include "MapchipHandler.h"
#include "Application/Utility/GameUtility.h"
#include "Application/Character/Player/Player.h"
#include "Application/Character/Child/Child.h"

#include <Engine/Assets/Animation/NodeAnimation/NodeAnimationPlayer.h>

void MapchipHandler::initialize(MapchipField* mapchipField) {
	mapchipField_ = mapchipField;
}

void MapchipHandler::update_player_on_mapchip(Player* player, Child* child) {
	// プレイヤーと子供が落下中かを判定
	check_fall_conditions(player, child);
}

int MapchipHandler::can_player_move_on_ice(Player* player, Child* child, const Vector3& direction) const {
	// マップチップを取得
	auto get_next_chip = [&](const Vector3& position) -> int {
		return mapchipField_->getElement(std::round(position.x), std::round(position.z));
	};

	// 移動量を宣言
	int moveNum = 1;

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
		return player->get_translate() + childDirection + direction * static_cast<float>(moveNum);
	};

	// 氷以外のブロックを見つけるまで探索
	while (true) {
		// 現在の移動量を取得
		//int moveNum = player->get_move_num_on_ice();
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
				//player->set_move_num_on_ice(moveNum);
				return moveNum;
			}
			// 壁だったら止まる
			if (nextChip == 2 || nextChildChip == 2) {
				moveNum -= 1;
				player->set_move_num_on_ice(moveNum);
				return moveNum;
			}
			// 床どちらも床だったら止まる
			if (nextChip == 1 && nextChildChip == 1) {
				return moveNum;
			}
			// 片方が床片方が床だったら
			if (nextChip == 0 && nextChildChip == 1 || nextChildChip == 3) {
				return moveNum;
			}
			// 子供が氷で且つプレイヤーが床だったら
			if (nextChildChip == 0 && nextChip == 1 || nextChip == 3) {
				return moveNum;
			}
			// 片方が床片方が床だったら
			if (nextChip == 4 && nextChildChip == 1 || nextChildChip == 3) {
				return moveNum;
			}
			// 子供が氷で且つプレイヤーが床だったら
			if (nextChildChip == 4 && nextChip == 1 || nextChip == 3) {
				return moveNum;
			}
		}
		// parentしていない場合
		else {
			Vector3 nextPosLeftSide = nextPos + GameUtility::rotate_direction_90_left(direction);
			Vector3 nextPosRightSide = nextPos + GameUtility::rotate_direction_90_right(direction);

			// 穴だったら移動しない
			if (nextChip == 0) {
				// そもそも移動できなくする処理
				//moveNum = 1;
				//return false;

				// 穴だったら止まる
				moveNum -= 1;
				//player->set_move_num_on_ice(moveNum);
				return moveNum;
			}
			// 壁だったら止まる
			if (nextChip == 2) {
				moveNum -= 1;
				//player->set_move_num_on_ice(moveNum);
				return moveNum;
			}

			// 移動中に子供とぶつかるか判定
			if (std::round(nextPos.x) == std::round(child->get_translate().x) &&
				std::round(nextPos.z) == std::round(child->get_translate().z)) {
				// 正面衝突する場合だったら1ます後ろにずらす
				moveNum -= 1;
				//player->set_move_num_on_ice(moveNum);
				return moveNum;
			}
			//　それ以外ならそのまま抜ける
			else if (std::round(nextPosLeftSide.x) == std::round(child->get_translate().x) &&
				std::round(nextPosLeftSide.z) == std::round(child->get_translate().z)) {
				return moveNum;
			}
			else if (std::round(nextPosRightSide.x) == std::round(child->get_translate().x) &&
				std::round(nextPosRightSide.z) == std::round(child->get_translate().z)) {
				return moveNum;
			}

			// 床だったらそのまま出る
			if (nextChip == 0) {
				return moveNum;
			}


		}
		moveNum += 1;
		//player->set_move_num_on_ice(moveNum);
	}
	return moveNum;
}

bool MapchipHandler::player_move_to_wall_or_holl(Player* player, Child* child, const Vector3& direction) const {
	// 落下中だったらモーションをしない
	if (player->is_falling()) return false;

	Vector3 nextPos = player->get_translate() + direction;
	if (!player->is_parent()) {
		auto element = mapchipField_->getElement(nextPos.x, nextPos.z);
		if (element == 0 || element == 2) { return true; }//移動先が壁か穴ならtrue
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

		// 移動先がどちらも穴だったらtrue
		if (elementNextPlayer == 0 && elementChild == 0) {
			player->set_on_ice(false);
			return true;
		}
		// 移動先のどちらかが壁だったらtrue
		if (elementNextPlayer == 2 || elementChild == 2) {
			player->set_on_ice(false);
			return true;
		}
	}
	return false;
}

bool MapchipHandler::can_player_move_to(Player* player, Child* child, const Vector3& direction) const {
	// 既に回転のほうが失敗していたら回転も失敗
	if (player->get_move_type() == MoveType::FallIntoHole ||
		player->get_move_type() == MoveType::HitRock) {
		return false;
	}

	// 落下中だったら移動不可
	if (player->is_falling()) return false;
	// プレイヤーの移動予定地
	Vector3 nextPos = player->get_translate() + direction;
	// 親子付けしていない場合
	if (!player->is_parent()) {
		// 移動後の座標が子供と重なってたら移動不可
		//if (nextPos == child->get_translate()) {
		if (GameUtility::approximately_equal(nextPos, child->get_translate(), 0.1f)) {
			//player->set_on_ice(false);
			//player->set_on_child(true);
			player->set_move_type(MoveType::MoveOnChild);
			return false;
		}
		// 移動先のマップチップ番号を取得
		auto element = mapchipField_->getElement(nextPos.x, nextPos.z);
		// 移動先が氷だったら氷上にいるフラグをオン
		if (element == 4) {
			player->set_move_type(MoveType::SlidingOnIce);
			return true;
		}
		// もし穴だったら
		if (element == 0) {
			player->set_move_type(MoveType::FallIntoHole);
			return false;
		}
		// もし壁だったら
		if (element == 2) {
			player->set_move_type(MoveType::HitRock);
			return false;
		}

		if (element == 1 || element == 3) {
			player->set_move_type(MoveType::Normal);
			return true;
		}
		// ここまで来たら普通に通る
		return true;
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
			player->set_move_type(MoveType::FallIntoHole);
			return false;
		}
		// 移動先のどちらかが壁だったらfalse
		if (elementNextPlayer == 2 || elementChild == 2) {
			player->set_move_type(MoveType::HitRock);
			return false;
		}
		// 移動先がどちらも氷またはどちらかが氷どちらかが穴の場合
		if ((elementNextPlayer == 4 || elementNextPlayer == 0) && (elementChild == 4 || elementChild == 0)) {
			player->set_move_type(MoveType::SlidingOnIce);
			return true;
		}
		// 特に何もなければ移動可能
		player->set_move_type(MoveType::Normal);
		return true;
	}
}

bool MapchipHandler::can_player_rotate(Player* player, Child* child, const Vector3& direction) const {
	Quaternion nextRotate = Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, direction);
	Quaternion nowRotate = player->get_rotation();

	if (std::abs(nextRotate.vector().y - nowRotate.vector().y) < 0.001) {
		player->set_rotate_type(RotateType::None);
		return false; // 同じ方向なら回転不要
	}
	// 今フレームで移動していたら抜ける(くっつく処理をしていた場合回転不要)
	if (player->is_move()) {
		//player->set_rotate_type(RotateType::None);
		return false;
	};
	// 落下中だったら回転不可
	if (player->is_falling()) return false;
	// 親子付けしていなければ可能
	if (!player->is_parent()) {
		player->set_rotate_type(RotateType::Normal);
		return true;
	}
	auto check_collision_wall = [&](const Vector3& pos) {
		return mapchipField_->getElement(std::round(pos.x), std::round(pos.z)) == 2;
	};

	auto check_collision_hole = [&](const Vector3& pos) {
		return mapchipField_->getElement(std::round(pos.x), std::round(pos.z)) == 0;
	};

	// 子供の向きを設定
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

	// 回転が90度ならばそれ用の処理をする
	if (GameUtility::IsRotation90Degrees(nowRotate, nextRotate)) {
		// 斜め前方向に障害物
		if (check_collision_wall(midChildPos)) {
			player->set_moving(false);
			// 回転失敗のフラグを立てておく
			player->set_turn_success(false);
			player->set_how_rotation(RotationDirection::Default);
			player->set_rotate_type(RotateType::Rotate90_HitObstacleDiagonalFront);
			return false;
		}
		// 移動先に障害物
		if (check_collision_wall(nextChildPos)) {
			player->set_moving(false);
			// 回転失敗のフラグを立てておく
			player->set_turn_success(false);
			player->set_how_rotation(RotationDirection::Default);
			player->set_rotate_type(RotateType::Rotate90_HitObstacleNextPosition);
			return false;
		}

		// 回転後に穴に落ちる場合は回転させない
		if (check_collision_hole(nowPlayerPos)) {
			if (check_collision_hole(nextChildPos)) {
				player->set_moving(false);
				// 回転失敗のフラグを立てておく
				player->set_turn_success(false);
				player->set_how_rotation(RotationDirection::Default);
				player->set_rotate_type(RotateType::Rotate90_NextPositionIsHole);
				return false;
			}
		}

		// ここまで来たら普通に回転できる
		player->set_rotate_type(RotateType::Rotate90_Normal);
		return true;
	}

	// ここまで来たら180度回転なのでそれ用の回転の変数を用意

	// 左右に障害物があるか判定
	auto check_side_collisions_wall = [&](const Vector3& primaryDirection, const Vector3& secondaryDirection) {
		Vector3 firstPos = nowPlayerPos + primaryDirection - childDirection;
		Vector3 secondPos = firstPos + secondaryDirection;
		Vector3 thirdPos = secondPos + secondaryDirection;
		return check_collision_wall(firstPos) || check_collision_wall(secondPos) || check_collision_wall(thirdPos);
	};

	// 左右に穴があるかを判定
	auto check_side_collision_hole = [&](const Vector3& primaryDirection, const Vector3& secondaryDirection) {
		Vector3 checkPos = nowPlayerPos + primaryDirection - childDirection + secondaryDirection;
		return check_collision_hole(checkPos);
	};

	Vector3 leftDirection = GameUtility::rotate_direction_90_left(childDirection);
	Vector3 rightDirection = GameUtility::rotate_direction_90_right(childDirection);

	// 左方向に障害物があれば
	if (check_side_collisions_wall(leftDirection, childDirection) ||
		(check_side_collision_hole(leftDirection, childDirection) && check_collision_hole(nowPlayerPos))) {
		player->set_how_rotation(RotationDirection::Right);
	}
	// 右方向に障害物があれば
	else if (check_side_collisions_wall(rightDirection, childDirection) ||
			(check_side_collision_hole(rightDirection, childDirection) && check_collision_hole(nowPlayerPos))) {
		player->set_how_rotation(RotationDirection::Left);
	}
	// どちらもなければ
	else {
		player->set_how_rotation(RotationDirection::Default);
	}


	auto check_collision = [&](const Vector3& pos) {
		return mapchipField_->getElement(std::round(pos.x), std::round(pos.z));
	};

	Vector3 firstCheckPos;
	Vector3 secondCheckPos;
	Vector3 thirdCheckPos;
	if (player->get_how_rotation() == RotationDirection::Left) {
		firstCheckPos = nowPlayerPos + leftDirection - childDirection;
		secondCheckPos = firstCheckPos + childDirection;
		thirdCheckPos = secondCheckPos + childDirection;
	}
	else {
		firstCheckPos = nowPlayerPos + rightDirection - childDirection;
		secondCheckPos = firstCheckPos + childDirection;
		thirdCheckPos = secondCheckPos + childDirection;
	}

	// 各探索座標の位置を取得
	int firstChip = check_collision(firstCheckPos);
	int secondChip = check_collision(secondCheckPos);
	int thirdChip = check_collision(thirdCheckPos);
	int finalChip = check_collision(nextChildPos);
	int playerChip = check_collision(nowPlayerPos);
	// 最初の探索点が壁だったら
	if (firstChip == 2) {
		player->set_rotate_type(RotateType::HitDiagonalFrontWall);
		return false; // 衝突があれば回転する
	}

	if (secondChip == 0 && playerChip == 0) {
		player->set_rotate_type(RotateType::NextTileIsHole);
		return false; // 衝突があれば回転する
	}

	if (secondChip == 2) {
		player->set_rotate_type(RotateType::HitSideWall);
		return false; // 衝突があれば回転する
	}

	if (thirdChip == 2) {
		player->set_rotate_type(RotateType::HitDiagonalBackWall);
		return false; // 衝突があれば回転する
	}

	if (finalChip == 0 && playerChip == 0) {
		player->set_rotate_type(RotateType::BackTileIsHole);
		return false; // 衝突があれば回転しない
	}

	if (finalChip == 2) {
		player->set_rotate_type(RotateType::HitBackWall);
		return false; // 衝突があれば回転しない
	}

	player->set_rotate_type(RotateType::Normal);
	return true; // 衝突がなければ回転を続行
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