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

	// くっつく処理
	if (player->get_transform().x + 1.0f == child->get_translate().x && player->get_transform().z == child->get_translate().z) {
		child->set_parent(player->get_object());
		child->set_translate({ 1.0f, 0.0f, 0.0f });
	}
	else if (player->get_transform().x - 1.0f == child->get_translate().x && player->get_transform().z == child->get_translate().z) {
		child->set_parent(player->get_object());
		child->set_translate({ -1.0f, 0.0f, 0.0f });
	}
	else if (player->get_transform().x == child->get_translate().x && player->get_transform().z + 1.0f == child->get_translate().z) {
		child->set_parent(player->get_object());
		child->set_translate({ 0.0f, 0.0f, 1.0f });
	}
	else if (player->get_transform().x == child->get_translate().x && player->get_transform().z - 1.0f == child->get_translate().z) {
		child->set_parent(player->get_object());
		child->set_translate({ 0.0f, 0.0f, -1.0f });
	}
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
