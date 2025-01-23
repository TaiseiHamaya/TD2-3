#include "Application/ClearUIClass.h"

#include "Engine/Module/World/Sprite/SpriteInstance.h"

ClearUIClass::ClearUIClass(){
    init();
}

ClearUIClass::~ClearUIClass(){}

void ClearUIClass::init(){
    clearFlag = false;
    sprite = std::make_unique<SpriteInstance>("ClearTex.png");


}

void ClearUIClass::update(){}

void ClearUIClass::begin_rendering(){ sprite->begin_rendering(); }

void ClearUIClass::darw(){ sprite->draw(); }
