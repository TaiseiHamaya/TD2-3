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

	wasdSprite[0] = std::make_unique<SpriteInstance>("Wkey.png");
	//wasdSprite[1] = std::make_unique<SpriteInstance>("Akey.png");
	//wasdSprite[2] = std::make_unique<SpriteInstance>("Skey.png");
	//wasdSprite[3] = std::make_unique<SpriteInstance>("Dkey.png");

	//wasdSprite[0]
	
}

void GameManagement::begin() {
	if (!(clearFlag || failedFlag)) {
		isReset = Input::IsTriggerKey(KeyID::R);
	}
}
#ifdef _DEBUG

#include <imgui.h>
void GameManagement::update(){

	ImGui::Begin("sprite");
	wasdSprite[0]->debug_gui();
	ImGui::End();
}
#endif
void GameManagement::begin_rendering(){
	clearSprite->begin_rendering();
	failedSprite->begin_rendering();
	wasdSprite[0]->begin_rendering();
}

void GameManagement::darw(){
	if(clearFlag)
	{
		clearSprite->draw();
	} else 	if(failedFlag)
	{
		failedSprite->draw();
	}
	wasdSprite[0]->draw();
}
