#pragma once
#include "PlayerStateBase.h"
class PlayerStateMoveCheck : public PlayerStateBase
{
public:
	PlayerStateMoveCheck() = default;
	~PlayerStateMoveCheck() override = default;
	void Enter(Player& player) override;
	void Update(Player& player) override;
	void Exit(Player& player) override;
};

