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
	Vector3 position = object_->get_transform().get_translate();
	if (Input::GetInstance().IsTriggerKey(KeyID::W)) {
		position.z += speed;
	}
	else if(Input::GetInstance().IsTriggerKey(KeyID::A)){
		position.x -= speed;
	}
	else if (Input::GetInstance().IsTriggerKey(KeyID::S)) {
		position.z -= speed;
	}
	else if (Input::GetInstance().IsTriggerKey(KeyID::D)) {
		position.x += speed;
	}
	object_->get_transform().set_translate(position);
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
