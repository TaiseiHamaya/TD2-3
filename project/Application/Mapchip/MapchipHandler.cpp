#include "MapchipHandler.h"
#include "Application/Utility/GameUtility.h"
#include "Application/Character/Player/Player.h"
#include "Application/Character/Child/Child.h"

void MapchipHandler::initialize(MapchipField* mapchipField) {
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

			// 移動中に子供とぶつかるか判定
			if (std::round(nextPos.x) == std::round(child->get_translate().x) &&
				std::round(nextPos.z) == std::round(child->get_translate().z)) {
				// 正面衝突する場合だったら1ます後ろにずらす
				moveNum -= 1;
				player->set_move_num_on_ice(moveNum);
				break;
			}
			//　それ以外ならそのまま抜ける
			else if (std::round(nextPosLeftSide.x) == std::round(child->get_translate().x) &&
				std::round(nextPosLeftSide.z) == std::round(child->get_translate().z)) {
				break;
			}
			else if (std::round(nextPosRightSide.x) == std::round(child->get_translate().x) &&
				std::round(nextPosRightSide.z) == std::round(child->get_translate().z)) {
				break;
			}

			// 床だったらそのまま出る
			if (nextChip == 0) {
				break;
			}


		}
		moveNum += 1;
		player->set_move_num_on_ice(moveNum);
	}
	return true;
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
	// 落下中だったら移動不可
	if (player->is_falling()) return false;

	// プレイヤーの移動予定地
	Vector3 nextPos = player->get_translate() + direction;
	// 親子付けしていない場合
	if (!player->is_parent()) {
		// 移動後の座標が子供と重なってたら移動不可
		if (nextPos == child->get_translate()) {
			player->set_on_ice(false);
			player->set_on_child(true);
			// 特例でtrue(移動不可なら本来はfalse)
			return true;
		}
		// 移動先のマップチップ番号を取得
		auto element = mapchipField_->getElement(nextPos.x, nextPos.z);
		// 移動先が氷だったら氷上にいるフラグをオン
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

bool MapchipHandler::can_player_rotate(Player* player, Child* child, const Vector3& direction) const {
	if (direction == player->get_previous_direction()) {
		return false; // 同じ方向なら回転不要
	}
	// 今フレームで移動していたら抜ける(くっつく処理をしていた場合回転不要)
	if (player->is_moved()) return false;
	// 落下中だったら回転不可
	if (player->is_falling()) return false;
	// 親子付けしていなければ可能
	if (!player->is_parent()) return true;

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

	// 一回転しない場合
	if (std::abs(direction.x + player->get_previous_direction().x) > 0.01f ||
		std::abs(direction.z + player->get_previous_direction().z) > 0.01f) {
		// 回転後に穴に落ちる場合は回転させない
		if (check_collision_hole(nowPlayerPos)) {
			if (check_collision_hole(nextChildPos)) {
				player->set_moving(false);
				// 回転失敗のフラグを立てておく
				player->set_turn_success(false);
				return false;
			}
		}

		// 障害物があったら移動フラグをオフにしとく
		if (check_collision_wall(nextChildPos) || check_collision_wall(midChildPos)) {
			player->set_moving(false);
			// 回転失敗のフラグを立てておく
			player->set_turn_success(false);
			return false;
		}
	}

	if (std::abs(direction.x + player->get_previous_direction().x) > 0.01f ||
		std::abs(direction.z + player->get_previous_direction().z) > 0.01f) {
		return true; // 180度ではない場合は回転可能
	}

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

	//// プレイヤーの座標が穴だったら
	//if (check_collision_hole(nowPlayerPos)) {
	//	// 回転終了予定地点が穴だったら抜ける
	//	if (check_collision_hole(nextChildPos)) {
	//		player->set_moving(false);
	//		player->set_turn_success(false);
	//		return false;
	//	}
	//	// 1回転するときは左右どちらも穴があったら抜ける
	//	if (check_side_collision_hole(leftDirection, childDirection)) {
	//		if (check_side_collision_hole(rightDirection, childDirection)) {
	//			player->set_moving(false);
	//			player->set_turn_success(false);
	//			return false;
	//		}
	//	}
	//}

	// 左方向に障害物があれば
	if (check_side_collisions_wall(leftDirection, childDirection) ||
		check_side_collision_hole(leftDirection, childDirection)) {
		player->set_how_rotation(RotationDirection::Right);
	}
	else {
		// 無ければ一旦左方向にしておく
		player->set_how_rotation(RotationDirection::Left);
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

	// 最初の探索点が壁だったら
	if (firstChip == 2) {

	}

	if (secondChip == 0) {

	}

	if (secondChip == 2) {

	}

	if (thirdChip == 2) {

	}

	if (finalChip == 0) {

	}

	if (finalChip == 2) {

	}
	//// 一回転するときは左右どちらも埋まってたら回転不可
	//if (check_side_collisions_wall(leftDirection, childDirection)) {
	//	if (check_side_collisions_wall(rightDirection, childDirection)) {
	//		player->set_moving(false);
	//		player->set_turn_success(false);
	//		return false;
	//	}
	//}

	return true; // 衝突がなければ回転を続行
}

void MapchipHandler::setup_rotation_parameters(Player* player, Child* child, const Vector3& direction) {
	if (!player->is_parent()) return;

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

	// 左方向
	Vector3 leftDirection = GameUtility::rotate_direction_90_left(childDirection);
	// 右方向
	Vector3 rightDirection = GameUtility::rotate_direction_90_right(childDirection);
	// 左斜め方向を計算
	Vector3 leftDiagonalDirection = (childDirection + leftDirection).normalize();
	// 右斜め方向を計算
	Vector3 rightDiagonalDirection = (childDirection + rightDirection).normalize();

	//auto check_collision = [&](const Vector3& pos) {
	//	return mapchipField_->getElement(std::round(pos.x), std::round(pos.z)) == 2;
	//};

	//auto check_collision_hole = [&](const Vector3& pos) {
	//	return mapchipField_->getElement(std::round(pos.x), std::round(pos.z)) == 0;
	//};



	//auto check_side_collisions = [&](const Vector3& primaryDirection, const Vector3& secondaryDirection) {
//	Vector3 firstPos = nowPlayerPos + primaryDirection - childDirection;
//	Vector3 secondPos = firstPos + secondaryDirection;
//	Vector3 thirdPos = secondPos + secondaryDirection;
//	return check_collision(firstPos) || check_collision(secondPos) || check_collision(thirdPos);
//};

//// 左右に穴があるかを判定
//auto check_side_collision_hole = [&](const Vector3& primaryDirection, const Vector3& secondaryDirection) {
//	Vector3 checkPos = nowPlayerPos + primaryDirection - childDirection + secondaryDirection;
//	return check_collision_hole(checkPos);
//};


	//// 移動の終了位置に障害物があるか
	//if (check_collision(nextChildPos)) {
	//	// 左右どちらに穴があるか判定
	//	bool holeLeft = check_side_collision_hole(leftDirection, childDirection);
	//	bool holeRight = check_side_collision_hole(rightDirection, childDirection);
	//	// 移動中に穴に落ちる場合の処理(左右どちらも計測)
	//	if (check_collision_hole(nowPlayerPos) && (holeLeft || holeRight)) {
	//		player->set_moving(false);
	//		player->set_reverse_rotation(Player::HowRotate::REVERSE);

	//		// 穴の位置に応じて微調整
	//		Vector3 targetDirection = childDirection;
	//		float angleBias = 0.01f; // 調整角度

	//		if (holeLeft) {
	//			// 左に穴がある場合、右へ少し回転
	//			Quaternion rotation = Quaternion::AngleAxis(Vector3{ 0, 1, 0 }, angleBias);
	//			targetDirection = targetDirection * rotation;
	//		}
	//		else if (holeRight) {
	//			// 右に穴がある場合、左へ少し回転
	//			Quaternion rotation = Quaternion::AngleAxis(Vector3{ 0, 1, 0 }, -angleBias);
	//			targetDirection = targetDirection * rotation;
	//		}

	//		// 回転設定
	//		Quaternion midRotate = Quaternion::FromToRotation(-childDirection, targetDirection);
	//		player->set_mid_rotation(midRotate);
	//		player->set_target_rotation(player->get_rotation());
	//		return;

	//	}
	//	else {
	//		player->set_moving(false);
	//		player->set_reverse_rotation(Player::HowRotate::REVERSE);
	//		if (std::round(child->get_translate().x) == 1.0f) {
	//			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, rightDirection));
	//		}
	//		else if (std::round(child->get_translate().x) == -1.0f) {
	//			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, leftDirection));
	//		}
	//		else {
	//			player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, childDirection));
	//		}
	//		player->set_target_rotation(player->get_rotation());
	//		return;
	//	}
	//}

	//// 移動の途中に障害物があるか
	//if (check_collision(midChildPos)) {
	//	player->set_moving(false);
	//	player->set_reverse_rotation(Player::HowRotate::REVERSE);
	//	if (std::round(child->get_translate().x) == 1.0f) {
	//		player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, rightDiagonalDirection));
	//	}
	//	else if (std::round(child->get_translate().x) == -1.0f) {
	//		player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, leftDiagonalDirection));
	//	}
	//	else {
	//		// プレイヤーと子が向かい合ってるときの斜め方向
	//		Vector3 childDiagonalDirection = (childDirection + player->get_previous_direction()).normalize();
	//		player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, childDiagonalDirection));
	//	}
	//	player->set_target_rotation(player->get_rotation());
	//	return;
	//}

	//if (std::abs(direction.x + player->get_previous_direction().x) > 0.01f ||
	//	std::abs(direction.z + player->get_previous_direction().z) > 0.01f) {
	//	return; // 180度ではない場合は回転可能
	//}

	//if (check_side_collisions(leftDirection, childDirection) || check_side_collision_hole(leftDirection, childDirection)) {
	//	// 左側が埋まってたら逆回転
	//	player->set_reverse_rotation(Player::HowRotate::RIGHT);
	//	if (std::round(child->get_translate().x) == 1.0f) {
	//		player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, -childDirection));
	//	}
	//	else if (std::round(child->get_translate().x) == -1.0f) {
	//		player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, childDirection));
	//	}
	//	else {
	//		player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, rightDirection));
	//	}
	//	return;
	//}

	//if (check_side_collisions(rightDirection, childDirection) || check_side_collision_hole(rightDirection, childDirection)) {
	//	// 右側が埋まってたら左回転
	//	player->set_reverse_rotation(Player::HowRotate::LEFT);
	//	if (std::round(child->get_translate().x) == 1.0f) {
	//		player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, childDirection));
	//	}
	//	else if (std::round(child->get_translate().x) == -1.0f) {
	//		player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, -childDirection));
	//	}
	//	else {
	//		player->set_mid_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, leftDirection));
	//	}
	//	return;
	//}
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
	// 前フレームと今フレームが違っていたらアニメーションの切り替え
	if (prePlayerChip != player->is_out_ground()) {
		if (player->is_out_ground()) {
			// 今フレームで下に足場がない状態だと浮いているので焦るモーション
			player->get_object()->reset_animated_mesh("ChiledKoala.gltf", "Flustered", true);
		}
		else {
			// 地面にいる場合は通常状態に戻す
			player->get_object()->reset_animated_mesh("ChiledKoala.gltf", "Standby", true);
		}
	}
	// プレイヤーと同じで子供が穴の上にいるかどうかが前フレームと今フレームで違っていたらアニメーションの切り替え
	if (preChildChip != child->is_out_ground()) {
		if (child->is_out_ground()) {
			// 空中にいるので焦るモーション
			child->get_object()->reset_animated_mesh("ChiledKoala.gltf", "Flustered", true);
		}
		else {
			// 地面にいるので抱きつきモーションに戻る
			child->get_object()->reset_animated_mesh("ChiledKoala.gltf", "Hold", false);
		}
	}

	// 親と繋がっているか判定
	if (!player->is_parent()) {
		// 繋がっていなければ別々で判定
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