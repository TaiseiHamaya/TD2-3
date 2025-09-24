#pragma once
#include "Application/Character/Player/State/PlayerStateBase.h"
#include <Application/Character/Player/Strategy/RotateStrategyBase.h>
class PlayerStateRotateCheck : public PlayerStateBase
{
public:
	PlayerStateRotateCheck() = default;
	~PlayerStateRotateCheck() override = default;
	void Enter(Player& player) override;
	void Update(Player& player) override;
	void Exit(Player& player) override;

private:
	Vector3 childDirection;
	std::unique_ptr<RotateStrategyBase> strategy_;
};

