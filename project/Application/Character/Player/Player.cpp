#include "Player.h"



void Player::load()
{
}

void Player::initialize()
{
	object_ = std::make_unique<MeshInstance>();
	object_->reset_mesh("Sphere.obj");
}

void Player::finalize()
{
}

void Player::begin()
{
}

void Player::update()
{
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
