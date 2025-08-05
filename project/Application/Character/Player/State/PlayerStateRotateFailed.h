#pragma once
#include "PlayerStateBase.h"
#include <Application/Character/Player/Player.h>
class PlayerStateRotateFailed : public PlayerStateBase
{
public:
	PlayerStateRotateFailed() = default;
	~PlayerStateRotateFailed() override = default;
	void Enter(Player& player) override;
	void Update(Player& player) override;
	void Exit(Player& player) override;
private:
	float rotateTimer = 0.0f;
	float rotateDuration = 0.15f;
};

