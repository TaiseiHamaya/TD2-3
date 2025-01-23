#include "Application/GameManagement.h"

#include "Engine/Module/World/Sprite/SpriteInstance.h"

GameManagement::GameManagement(){
	init();
}

GameManagement::~GameManagement(){}

void GameManagement::init(){
	clearFlag = false;
	failedFlag = false;
	clearSprite = std::make_unique<SpriteInstance>("ClearTex.png");
	failedSprite = std::make_unique<SpriteInstance>("FailedTex.png");

}

void GameManagement::update(){}

void GameManagement::begin_rendering(){
	clearSprite->begin_rendering();
	failedSprite->begin_rendering();
}

void GameManagement::darw(){
	if(clearFlag)
	{
		clearSprite->draw();
	} else 	if(failedFlag)
	{
		failedSprite->draw();
	}
}
