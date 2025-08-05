#pragma once
#include "PlayerStateBase.h"
#include <memory>
#include "Engine/Resources/Audio/AudioPlayer.h"
class PlayerStateRotate : public PlayerStateBase
{
public:
	PlayerStateRotate();
	~PlayerStateRotate() override = default;
	void Enter(Player& player) override;
	void Update(Player& player) override;
	void Exit(Player& player) override;

private:
	float rotateTimer = 0.0f;
	float rotateDuration = 0.15f;
	std::unique_ptr<AudioPlayer> rotatAudio;//回転時間
};

