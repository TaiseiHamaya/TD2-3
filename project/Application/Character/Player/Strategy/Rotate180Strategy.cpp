#include "Rotate180Strategy.h"
#include "Application/Character/Player/Player.h"
#include <Application/Utility/GameUtility.h>
#include <numbers>

bool Rotate180Strategy::CanRotate(Player& player, const Vector3& childDirection) const {
	// 移動中・落下中は回転不可
	if (player.is_move() || player.is_falling()) return false;

	// 親子付けしてなければ普通に回転
	if (!player.is_parent()) {
		player.set_rotate_type(RotateType::Normal);
		player.set_start_rotation(player.get_rotation());
		player.set_target_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, player.get_move_direction()));
		player.change_state("Rotate");
		return true;
	}

	auto is_wall = [&](const Vector3& pos) {
		return player.get_mapchip()->getElement(std::round(pos.x), std::round(pos.z)) == 2;
	};
	auto is_hole = [&](const Vector3& pos) {
		return player.get_mapchip()->getElement(std::round(pos.x), std::round(pos.z)) == 0;
	};

	Vector3 nowPlayerPos = player.get_translate();
	Vector3 nowChildPos = nowPlayerPos + player.get_child()->get_translate() * player.get_rotation();
	Vector3 nextChildPos = nowPlayerPos + childDirection;

	// 左右確認用
	Vector3 leftDirection = GameUtility::rotate_direction_90_left(childDirection);
	Vector3 rightDirection = GameUtility::rotate_direction_90_right(childDirection);

	// 左右に障害物があるか
	auto check_side_collisions_wall = [&](const Vector3& sideDir, const Vector3& forwardDir) {
		Vector3 first = nowPlayerPos + sideDir - childDirection;
		Vector3 second = first + forwardDir;
		Vector3 third = second + forwardDir;
		return is_wall(first) || is_wall(second) || is_wall(third);
	};

	// 左右に穴があるか
	auto check_side_collisions_hole = [&](const Vector3& sideDir, const Vector3& forwardDir) {
		Vector3 checkPos = nowPlayerPos + sideDir - childDirection + forwardDir;
		return is_hole(checkPos);
	};

	// 左優先
	if (check_side_collisions_wall(leftDirection, childDirection) ||
		(check_side_collisions_hole(leftDirection, childDirection) && is_hole(nowPlayerPos))) {
		player.set_how_rotation(RotationDirection::Right);
	}
	else if (check_side_collisions_wall(rightDirection, childDirection) ||
			(check_side_collisions_hole(rightDirection, childDirection) && is_hole(nowPlayerPos))) {
		player.set_how_rotation(RotationDirection::Left);
	}
	else {
		player.set_how_rotation(RotationDirection::Default);
	}

	// 経路判定
	auto check_chip = [&](const Vector3& pos) {
		return player.get_mapchip()->getElement(std::round(pos.x), std::round(pos.z));
	};

	Vector3 firstPos, secondPos, thirdPos;
	if (player.get_how_rotation() == RotationDirection::Left) {
		firstPos = nowPlayerPos + leftDirection - childDirection;
		secondPos = firstPos + childDirection;
		thirdPos = secondPos + childDirection;
	}
	else {
		firstPos = nowPlayerPos + rightDirection - childDirection;
		secondPos = firstPos + childDirection;
		thirdPos = secondPos + childDirection;
	}

	player.set_start_rotation(player.get_rotation());
	player.set_target_rotation(player.get_rotation());

	// 各探索座標の位置を取得
	int firstChip = check_chip(firstPos);
	int secondChip = check_chip(secondPos);
	int thirdChip = check_chip(thirdPos);
	int finalChip = check_chip(nextChildPos);
	int playerChip = check_chip(nowPlayerPos);


	// 回転失敗処理用
	auto set_failed_mid_rotation = [&](RotateType type, float angleDegree, const Vector3& axis) {
		player.set_rotate_type(type);

		// 回転軸がゼロに近いときはY軸にする
		Vector3 safeAxis = axis.length() < 0.0001f ? Vector3(0, 1, 0) : axis.normalize_safe();

		float angleRad = angleDegree * (std::numbers::pi_v<float> / 180.0f);
		Quaternion stepRotation = Quaternion::AngleAxis(safeAxis, angleRad);
		player.set_mid_rotation(stepRotation * player.get_rotation());
	};

	// 回転軸取得
	auto get_rotation_axis = [&]() {
		Vector3 forward = Vector3(0, 0, -1) * player.get_rotation();
		Vector3 moveDir = player.get_move_direction();
		Vector3 axis = Vector3::CrossProduct(forward, moveDir);
		return axis;
	};

	Vector3 rotationAxis = get_rotation_axis();

	// 判定順 + mid回転
	if (firstChip == 2) {
		set_failed_mid_rotation(RotateType::HitDiagonalFrontWall, -15.0f, rotationAxis);
		return false;
	}

	if (secondChip == 0 && playerChip == 0) {
		set_failed_mid_rotation(RotateType::NextTileIsHole, -60.0f, rotationAxis);
		return false;
	}

	if (secondChip == 2) {
		set_failed_mid_rotation(RotateType::HitSideWall, -60.0f, rotationAxis);
		return false;
	}

	if (thirdChip == 2) {
		set_failed_mid_rotation(RotateType::HitDiagonalBackWall, -105.0f, rotationAxis);
		return false;
	}

	if (finalChip == 0 && playerChip == 0) {
		set_failed_mid_rotation(RotateType::BackTileIsHole, -150.0f, rotationAxis);
		return false;
	}

	if (finalChip == 2) {
		set_failed_mid_rotation(RotateType::HitBackWall, -150.0f, rotationAxis);
		return false;
	}

	// ここまで来たら回転できるので設定
	Quaternion startRotation = player.get_rotation();
	Quaternion endRotation = Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, player.get_move_direction());
	//Quaternion midRotation = endRotation * startRotation.inverse();
	Quaternion midRotation = Quaternion::Slerp(startRotation, endRotation, 0.5f);

	// ここで回転をセット
	player.set_start_rotation(startRotation);
	player.set_target_rotation(endRotation);
	player.set_mid_rotation(midRotation);

	player.set_rotate_type(RotateType::Normal);
	player.change_state("Rotate");
	return true; // 衝突がなければ回転を続行
}
