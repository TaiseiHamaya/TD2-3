#include "Player.h"
#include "Engine/Runtime/Input/Input.h"


void Player::load()
{
}

void Player::initialize()
{
	object_ = std::make_unique<MeshInstance>();
	object_->reset_mesh("ParentObj.obj");
	object_->get_transform().set_translate({2.0f, 1.0f, 2.0f});
}

void Player::finalize()
{
}

void Player::begin()
{
}

void Player::update()
{
	move();
	rotate();
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

void Player::move()
{
	// このフレームで移動したかどうかの判定
	isMove = false;
	Vector3 position = object_->get_transform().get_translate();
	if (Input::GetInstance().IsTriggerKey(KeyID::W)) {
		position.z += speed;
		isMove = true;
		direction = { 0.0f, 0.0f, 1.0f };
	}
	else if (Input::GetInstance().IsTriggerKey(KeyID::A)) {
		position.x -= speed;
		isMove = true;
		direction = { -1.0f, 0.0f, 0.0f };
	}
	else if (Input::GetInstance().IsTriggerKey(KeyID::S)) {
		position.z -= speed;
		isMove = true;
		direction = { 0.0f, 0.0f, -1.0f };
	}
	else if (Input::GetInstance().IsTriggerKey(KeyID::D)) {
		position.x += speed;
		isMove = true;
		direction = { 1.0f, 0.0f, 0.0f };
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

