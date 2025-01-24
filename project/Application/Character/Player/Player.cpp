#include "Player.h"
#include "Engine/Runtime/Input/Input.h"


void Player::initialize()
{
	object_ = std::make_unique<MeshInstance>();
	object_->reset_mesh("ParentObj.obj");
	object_->get_transform().set_translate({ 3.0f, 1.0f, 1.0f });
	targetPosition = object_->get_transform().get_translate();
}

void Player::finalize()
{
}

void Player::update()
{
	if (isFall) {
		Vector3 position = object_->get_transform().get_translate();
		position.y -= 0.1f;
		object_->get_transform().set_translate(position);
	}
}

void Player::update(MapchipField* mapchipField)
{
	move(mapchipField);

	preDirection = direction;
}

void Player::begin_rendering()
{
	object_->begin_rendering();
}

void Player::draw() const
{
	object_->draw();
}

#ifdef _DEBUG
void Player::debug_update()
{
	ImGui::Begin("Player");
	object_->debug_gui();
	ImGui::End();
}
#endif // _DEBUG

void Player::move(MapchipField* mapchipField)
{
	// このフレームで移動したかどうかの判定(毎フレームfalseにする)
	isMove = false;

	// 回転が終わっていない場合は回転のみ処理
	if (isRotating) {
		rotate(mapchipField);
		return;
	}

	if (isMoving) {
		// 移動中なら補間処理を実行
		moveTimer += deltaTime;

		if (moveTimer >= moveDuration) {
			// 移動完了
			moveTimer = moveDuration;
			isMoving = false;
			isMove = true;
		}

		// 現在の位置を補間
		Vector3 position = Vector3::Lerp(object_->get_transform().get_translate(), targetPosition, moveTimer / moveDuration);
		object_->get_transform().set_translate(position);
		return;
	}

	Vector3 directions[] = {
	{ 0.0f, 0.0f, 1.0f },  // W: 前
	{ -1.0f, 0.0f, 0.0f }, // A: 左
	{ 0.0f, 0.0f, -1.0f }, // S: 後
	{ 1.0f, 0.0f, 0.0f }   // D: 右
	};

	KeyID keys[] = { KeyID::W, KeyID::A, KeyID::S, KeyID::D };

	for (size_t i = 0; i < 4; ++i) {
		if (Input::GetInstance().IsTriggerKey(keys[i])) {

			direction = directions[i];

			// 回転の準備
			start_rotation(mapchipField);

			// 次の目標位置
			Vector3 nextPos = object_->get_transform().get_translate() + direction;

			// 移動可能かを判定
			if (can_move_to(nextPos, mapchipField) &&
				check_collision_during_translation(mapchipField)) {
				// 移動の準備
				targetPosition = nextPos;
				moveTimer = 0.0f;
			}
			else {
				// 移動不可なら目的地は現在地に同じ
				targetPosition = object_->get_transform().get_translate();
			}
			// 回転は常に開始し、ここで終了
			return;
		}
	}
}

void Player::rotate(MapchipField* mapchipField)
{
	// 回転しているか判定
	if (isRotating) {
		rotateTimer += deltaTime;

		// 回転完了チェック
		if (rotateTimer >= rotateDuration) {
			rotateTimer = rotateDuration;
			isRotating = false;
			isMoving = true;
		}

		// 全体の進行度
		float totalProgress = rotateTimer / rotateDuration;

		Quaternion currentRotation;

		// 回転方向が逆の場合、進行度を反転し区間ごとに補間
		if (isReverseRotation) {
			if (totalProgress <= 0.5f) {
				// 前半区間（start → mid）
				float t = totalProgress / 0.5f; // 正規化した進行度
				currentRotation = Quaternion::Slerp(startRotation, midRotation, t);
			}
			else {
				// 後半区間（mid → target）
				float t = (totalProgress - 0.5f) / 0.5f; // 正規化した進行度
				currentRotation = Quaternion::Slerp(midRotation, targetRotation, t);
			}
		}
		else {
			// 通常の回転（start → target）
			float t = totalProgress;
			currentRotation = Quaternion::Slerp(startRotation, targetRotation, t);
		}

		// 現在の回転を設定
		object_->get_transform().set_quaternion(currentRotation);
	}
}

bool Player::can_move_to(const Vector3& position, MapchipField* mapchipField) const
{
	auto element = mapchipField->getElement(position.x, position.z);
	return element == 1 || element == 3;
}

void Player::start_rotation(MapchipField* mapchipField)
{
	startRotation = object_->get_transform().get_quaternion();
	targetRotation = Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, direction);
	midRotation = targetRotation;
	rotateTimer = 0.0f;
	isRotating = true;
	isReverseRotation = false;

	// 回転中の衝突チェック
	if (check_collision_during_rotation(mapchipField)) {
		return; // 衝突した場合は処理を中断
	}
}

bool Player::check_collision_during_rotation(MapchipField* mapchipField)
{
	if (!isRotating || !isParent) {
		return false; // 回転していない場合や子供がいない場合は何もしない
	}

	// 回転が必要であれば
	if (direction == preDirection) {
		return false;
	}

	// 今の子供の位置
	Vector3 nowChildPos = object_->get_transform().get_translate() + child_->get_translate() * startRotation;
	// 移動予定の位置
	Vector3 nextChildPos = object_->get_transform().get_translate() + direction;

	// ゴールの位置にブロックが
	if (mapchipField->getElement(std::round(nextChildPos.x), std::round(nextChildPos.z)) == 2) {
		// 壁に衝突した場合は回転を中断
		rotateTimer = 0.0f;
		isRotating = false;
		object_->get_transform().set_quaternion(startRotation); // 元の回転に戻す
		return true;
	}

	// 一回転しない場合の経由点
	Vector3 midChildPos = nowChildPos + direction;

	if (mapchipField->getElement(std::round(midChildPos.x), std::round(midChildPos.z)) == 2) {
		// 壁に衝突した場合は回転を中断
		rotateTimer = 0.0f;
		isRotating = false;
		object_->get_transform().set_quaternion(startRotation); // 元の回転に戻す
		return true;
	}

	Vector3 leftDirection = rotate_direction_90_left(direction);
	Vector3 rightDirection = rotate_direction_90_right(direction);
	Vector3 leftMidPos = object_->get_transform().get_translate() + leftDirection;
	Vector3 rightMidPos = object_->get_transform().get_translate() + rightDirection;

	if (mapchipField->getElement(std::round(leftMidPos.x), std::round(leftMidPos.z)) == 2) {
		// 左側が埋まってたら逆回転
		isReverseRotation = true;
		midRotation = Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, rightDirection);
		if (mapchipField->getElement(std::round(rightMidPos.x), std::round(rightMidPos.z)) == 2) {
			// 両方埋まってたら終了
			rotateTimer = 0.0f;
			isRotating = false;
			object_->get_transform().set_quaternion(startRotation); // 元の回転に戻す
			return true;
		}
	}

	if (mapchipField->getElement(std::round(rightMidPos.x), std::round(rightMidPos.z)) == 2) {
		// 右側が埋まってたら逆回転
		isReverseRotation = true;
		midRotation = Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, leftDirection);
		if (mapchipField->getElement(std::round(leftMidPos.x), std::round(leftMidPos.z)) == 2) {
			// 両方埋まってたら終了
			rotateTimer = 0.0f;
			isRotating = false;
			object_->get_transform().set_quaternion(startRotation); // 元の回転に戻す
			return true;
		}
	}

	return false; // 衝突がなければ回転を続行
}

bool Player::check_collision_during_translation(MapchipField* mapchipField)
{
	if (isMoving || !isParent) {
		return true; // 移動していない場合や子供がいない場合はそのまま通す
	}

	Vector3 nextPos = object_->get_transform().get_translate() + (direction * 2);

	if (mapchipField->getElement(nextPos.x, nextPos.z) != 2) {
		return true;
	}
	return false;
}

bool Player::approximately_equal(const Vector3& a, const Vector3& b, float epsilon)
{
	return std::fabs(a.x - b.x) < epsilon &&
		std::fabs(a.y - b.y) < epsilon &&
		std::fabs(a.z - b.z) < epsilon;
}

Vector3 Player::rotate_direction_90_left(const Vector3& direction)
{
	return { -direction.z, direction.y, direction.x }; // 左回転
}

Vector3 Player::rotate_direction_90_right(const Vector3& direction)
{
	return { direction.z, direction.y, -direction.x }; // 右回転
}

