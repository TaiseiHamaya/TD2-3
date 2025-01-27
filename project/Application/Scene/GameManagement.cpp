#include "Application/Scene/GameManagement.h"

#include "Engine/Module/World/Sprite/SpriteInstance.h"
#include <Engine/Runtime/Input/Input.h>

GameManagement::GameManagement(){
	init();
}

GameManagement::~GameManagement(){}

void GameManagement::init(){
	clearFlag = false;
	failedFlag = false;
	isReset = false;
	clearSprite = std::make_unique<SpriteInstance>("ClearTex.png");
	failedSprite = std::make_unique<SpriteInstance>("FailedTex.png");


	
}

void GameManagement::begin() {
	if (!(clearFlag || failedFlag)) {
		isReset = Input::IsTriggerKey(KeyID::R);
	}
}

void GameManagement::update(){

	
}

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
