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
	rotate();

	if (isFall) {
		Vector3 position = object_->get_transform().get_translate();
		position.y -= 0.01f;
		object_->get_transform().set_translate(position);
	}
}

void Player::update(MapchipField* mapchipField)
{
	move(mapchipField);
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
		rotate();
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

			// 回転の準備
			start_rotation(directions[i]);

			// 次の目標位置
			Vector3 nextPos = object_->get_transform().get_translate() + directions[i];

			// 移動可能かを判定
			if (can_move_to(nextPos, mapchipField)) {
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

void Player::rotate()
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
		// 補間して現在の回転を計算
		auto currentRotation = Quaternion::Slerp(startRotation, targetRotation, rotateTimer / rotateDuration);
		object_->get_transform().set_quaternion(currentRotation);
	}
}

bool Player::can_move_to(const Vector3& position, MapchipField* mapchipField) const
{
	auto element = mapchipField->getElement(position.x, position.z);
	return element == 1 || element == 3;
}

void Player::start_rotation(const Vector3& direction)
{
	startRotation = object_->get_transform().get_quaternion();
	targetRotation = Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, direction);
	rotateTimer = 0.0f;
	isRotating = true;
}

