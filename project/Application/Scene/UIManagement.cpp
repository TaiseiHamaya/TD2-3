#include "Application/Scene/UIManagement.h"

#include "Engine/Module/World/Sprite/SpriteInstance.h"
#include <Engine/Runtime/Input/Input.h>

UIManagement::UIManagement(){
	init();
}

UIManagement::~UIManagement(){}

void UIManagement::init(){
	clearFlag = false;
	failedFlag = false;
	isReset = false;
	clearSprite = std::make_unique<SpriteInstance>("ClearTex.png");
	failedSprite = std::make_unique<SpriteInstance>("FailedTex.png");

	wasdSprite[0] = std::make_unique<SpriteInstance>("Wkey.png");

}

void UIManagement::begin() {
	if (!(clearFlag || failedFlag)) {
		isReset = Input::IsTriggerKey(KeyID::R);
	}
}

void UIManagement::update(){}

void UIManagement::begin_rendering(){
	clearSprite->begin_rendering();
	failedSprite->begin_rendering();
}

void UIManagement::darw(){
	if(clearFlag)
	{
		clearSprite->draw();
	} else 	if(failedFlag)
	{
		failedSprite->draw();
	}
}
