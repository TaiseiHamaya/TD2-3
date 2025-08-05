#pragma once
#include "Application/Character/Player/State/PlayerStateBase.h"
class PlayerStateIdle : public PlayerStateBase
{
public:
	PlayerStateIdle() = default;
	~PlayerStateIdle() override = default;
	void Enter(Player& player) override;
	void Update(Player& player) override;
	void Exit(Player& player) override;
};

