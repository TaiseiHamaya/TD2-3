#pragma once
#include "PlayerStateBase.h"
class PlayerStateIceCheck : public PlayerStateBase
{
public:
	PlayerStateIceCheck() = default;
	~PlayerStateIceCheck() override = default;
	void Enter(Player& player) override;
	void Update(Player& player) override;
	void Exit(Player& player) override;

private:
	int moveNum;
};

