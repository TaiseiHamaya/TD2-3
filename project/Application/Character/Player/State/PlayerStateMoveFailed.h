#pragma once
#include "PlayerStateBase.h"
class PlayerStateMoveFailed : public PlayerStateBase
{
public:
	PlayerStateMoveFailed() = default;
	~PlayerStateMoveFailed() override = default;
	void Enter(Player& player) override;
	void Update(Player& player) override;
	void Exit(Player& player) override;
private:
	float wallMoveTimer;
};

