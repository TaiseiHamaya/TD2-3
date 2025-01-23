#pragma once
#include <memory>
#include "Application/Character/Player/Player.h"
#include "Application/Character/Player/Child/Child.h"
class PlayerManager
{
public:
	void initialize();
	void finalize();

	void begin();
	void update();
	void begin_rendering();
	void late_update();

	void draw() const;

private:
	std::unique_ptr<Player> player;
	std::unique_ptr<Child> child;

};

