#include "PlayerManager.h"

void PlayerManager::initialize()
{
	player = std::make_unique<Player>();
	player->initialize();
	child = std::make_unique<Child>();
	child->initialize();
}

void PlayerManager::finalize()
{
}

void PlayerManager::begin()
{
}

void PlayerManager::update()
{
	player->update();
	child->update();


	if (player->get_transform() + {1.0f, 0.0f, 0.0f})
}

void PlayerManager::begin_rendering()
{
	player->begin_rendering();
	child->begin_rendering();
}

void PlayerManager::late_update()
{
}

void PlayerManager::draw() const
{
	player->draw();
	child->draw();
}
