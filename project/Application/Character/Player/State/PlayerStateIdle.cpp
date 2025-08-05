#include "PlayerStateIdle.h"
#include "Application/Character/Player/Player.h"

void PlayerStateIdle::Enter(Player& player)
{
	player.set_how_rotation(RotationDirection::Default);
	//rotateType = RotateType::None;
	//moveType = MoveType::Normal;
	//exclamationData_.timer = 0.0f;
}

void PlayerStateIdle::Update(Player& player)
{

}

void PlayerStateIdle::Exit(Player& player)
{

}
