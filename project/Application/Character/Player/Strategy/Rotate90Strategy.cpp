#include "Rotate90Strategy.h"
#include "Application/Character/Player/Player.h"
#include <numbers>

bool Rotate90Strategy::CanRotate(Player& player, const Vector3& childDirection) const {
	// 今フレームで移動していたら抜ける(くっつく処理をしていた場合回転不要)
	if (player.is_move()) {
		player.change_state("Idle");
		return false;
	};
	// 落下中だったら回転不可
	if (player.is_falling()) {
		player.change_state("Idle");
		return false;
	}
	// 親子付けしていなければ可能
	if (!player.is_parent()) {
		player.set_rotate_type(RotateType::Normal);
		// 回転の始点と終点を設定
		player.set_start_rotation(player.get_rotation());
		player.set_target_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, player.get_move_direction().normalize()));

		player.change_state("Rotate");
		return true;
	}

	auto check_collision_wall = [&](const Vector3& pos) {
		return player.get_mapchip()->getElement(std::round(pos.x), std::round(pos.z)) == 2;
	};

	auto check_collision_hole = [&](const Vector3& pos) {
		return player.get_mapchip()->getElement(std::round(pos.x), std::round(pos.z)) == 0;
	};

	// 現在のプレイヤーの位置
	Vector3 nowPlayerPos = player.get_translate();
	// 今の子供のトランスレート
	Vector3 childTranslate = player.get_child()->get_translate();
	// プレイヤーの更新
	Quaternion playerRotate = player.get_rotation();
	// 今の子供の位置
	Vector3 nowChildPos = nowPlayerPos + childTranslate * playerRotate;
	// 移動予定の位置
	Vector3 nextChildPos = nowPlayerPos + childDirection;
	// 一回転しない場合の経由点
	Vector3 midChildPos = nowChildPos + childDirection;

	bool blockedMid = check_collision_wall(midChildPos);
	bool blockedNext = check_collision_wall(nextChildPos);
	bool holeInNext = check_collision_hole(nextChildPos);
	bool holeInNow = check_collision_hole(nowPlayerPos);

	bool shortestPathClear = !blockedMid && !blockedNext && (!holeInNext || !holeInNow);

	if (shortestPathClear) {
		player.set_how_rotation(RotationDirection::Default);
		// ここまで来たら普通に回転できる
		player.set_rotate_type(RotateType::Rotate90_Normal);
		// 回転の始点と終点を設定
		player.set_start_rotation(player.get_rotation());
		player.set_target_rotation(Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, player.get_move_direction()));
		
		player.change_state("Rotate");
		return true;
	}

	/// ここまできたら反対回転の検証

	// 逆回転の経由点
	std::vector<Vector3> reversePath;
	reversePath.resize(6);

	// 経由点を一つずつ設定していく
	reversePath[0] = nowChildPos - childDirection;
	reversePath[1] = nowPlayerPos - childDirection;
	reversePath[2] = reversePath[1] - reversePath[0];
	reversePath[2].y = 1.0f;
	reversePath[3] = reversePath[2] + childDirection;
	reversePath[4] = reversePath[3] + childDirection;
	reversePath[5] = nextChildPos;

	bool reversePathClear = true;
	for (const auto& pos : reversePath) {
		if (check_collision_wall(pos)) {
			reversePathClear = false;
			break;
		}
	}

	if (check_collision_hole(nowPlayerPos)) {
		if (check_collision_hole(nextChildPos)) {
			reversePathClear = false;
		}
		else if (check_collision_hole(reversePath[1])) {
			reversePathClear = false;
		}
		else if (check_collision_hole(reversePath[3])) {
			reversePathClear = false;
		}

	}

	// 逆回転もOKなら270度回転扱いで成功
	if (reversePathClear) {
		Quaternion startRotation = player.get_rotation();
		Quaternion endRotation = Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, player.get_move_direction());
		Quaternion midRotation = endRotation * startRotation.inverse();

		// ここで回転をセット
		player.set_start_rotation(startRotation);
		player.set_target_rotation(endRotation);
		player.set_mid_rotation(Quaternion::Slerp(startRotation, endRotation, 0.5f));

		player.set_rotate_type(RotateType::Rotate90_Reverce);
		player.set_how_rotation(RotationDirection::Reverce); // 逆回転用

		player.change_state("Rotate");
		return true;
	}

	/// 以降は回転失敗処理

	player.set_rotate_type(RotateType::Rotate90_HitObstacleDiagonalFront);
	player.set_how_rotation(RotationDirection::Default);
	player.set_turn_success(false);

	Quaternion startRotation = player.get_rotation();
	Quaternion endRotation = startRotation;

	player.set_start_rotation(startRotation);
	player.set_target_rotation(endRotation);

	// 回転軸を求める (start → target の回転軸)
	Vector3 startForward = Vector3(0.0f, 0.0f, -1.0f) * startRotation;
	Vector3 targetForward = Vector3(0.0f, 0.0f, -1.0f) * endRotation;

	Vector3 rotationAxis = Vector3::CrossProduct(startForward, targetForward).normalize_safe();

	Quaternion stepRotation;

	// もし軸が求まらなかったらY軸回転にする
	if (rotationAxis.length() < 0.0001f) {
		
	}
	rotationAxis = { 0.0f, 1.0f, 0.0f };
	if (blockedMid) {
		const float rotateAngle = 345.0f * (std::numbers::pi_v<float> / 180.0f);
		// 15度だけ回したクォータニオン
		stepRotation = Quaternion::AngleAxis(rotationAxis, rotateAngle);
	}
	else {
		const float rotateAngle = 300.0f * (std::numbers::pi_v<float> / 180.0f);
		// 15度だけ回したクォータニオン
		stepRotation = Quaternion::AngleAxis(rotationAxis, rotateAngle);
	}

	// 中間回転 = startRotation に stepRotation をかけたもの
	Quaternion midRotation = stepRotation * player.get_rotation();

	player.set_mid_rotation(midRotation);

	// midRotateをぶつかる向きに合わせて設定（例：最短回転側でぶつかった壁の向き）
	//player.set_mid_rotation(Quaternion::FromToRotation({ 0, 0, -1 }, childDirection));

	player.change_state("RotateFailed");
	return false;
}
