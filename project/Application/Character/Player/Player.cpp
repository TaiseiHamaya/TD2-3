#include "Player.h"
#include "Engine/Runtime/Input/Input.h"

void Player::initialize()
{
	object_ = std::make_unique<MeshInstance>();
	object_->reset_mesh("ParentObj.obj");
	object_->get_transform().set_translate({ 2.0f, 1.0f,2.0f });
}

void Player::finalize()
{
}

void Player::begin()
{
}

void Player::update(MapchipField* mapchipField)
{
	move(mapchipField);
	rotate();

	if (isFall) {
		Vector3 position = object_->get_transform().get_translate();
		position.y -= 0.1f;
		object_->get_transform().set_translate(position);
	}
}

void Player::begin_rendering()
{
	object_->begin_rendering();
}

void Player::late_update()
{
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
	// このフレームで移動したかどうかの判定
	isMove = false;
	Vector3 position = object_->get_transform().get_translate();
	if (Input::IsTriggerKey(KeyID::W)) {
		if (mapchipField->getElement(position.x, position.z + 1.0f) == 1) {
			position.z += speed;
			isMove = true;
			direction = { 0.0f, 0.0f, 1.0f };
		}
	}
	else if (Input::IsTriggerKey(KeyID::A)) {
		if (mapchipField->getElement(position.x - 1.0f, position.z) == 1) {
			position.x -= speed;
			isMove = true;
			direction = { -1.0f, 0.0f, 0.0f };
		}
	}
	else if (Input::IsTriggerKey(KeyID::S)) {
		if (mapchipField->getElement(position.x, position.z - 1.0f) == 1) {
			position.z -= speed;
			isMove = true;
			direction = { 0.0f, 0.0f, -1.0f };
		}
	}
	else if (Input::IsTriggerKey(KeyID::D)) {
		if (mapchipField->getElement(position.x + 1.0f, position.z) == 1) {
			position.x += speed;
			isMove = true;
			direction = { 1.0f, 0.0f, 0.0f };
		}
	}
	object_->get_transform().set_translate(position);
}

void Player::rotate()
{
	Quaternion rotation = object_->get_transform().get_quaternion();
	// 前方向
	Vector3 forwardDirection = Vector3(0.0f, 0.0f, -1.0f);
	rotation = Quaternion::FromToRotation(direction, Vector3(0.0f, 0.0f, -1.0f));

	object_->get_transform().set_quaternion(rotation);
}

