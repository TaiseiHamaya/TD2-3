#include "GameSceneUI.h"
#include "Engine/Module/World/Sprite/SpriteInstance.h"
#include <Engine/Runtime/Input/Input.h>
GameSceneUI::GameSceneUI(){ init(); }

GameSceneUI::~GameSceneUI(){}

void GameSceneUI::init(){
	wasdSprite[0] = std::make_unique<SpriteInstance>("Wkey.png");
	//wasdSprite[1] = std::make_unique<SpriteInstance>("Akey.png");
	//wasdSprite[2] = std::make_unique<SpriteInstance>("Skey.png");
	//wasdSprite[3] = std::make_unique<SpriteInstance>("Dkey.png");

	//wasdSprite[0]

}
#ifdef _DEBUG

#include <imgui.h>
void GameSceneUI::update(){

	ImGui::Begin("sprite");
	wasdSprite[0]->debug_gui();
	ImGui::End();
}
#endif
void GameSceneUI::begin_rendering(){
	wasdSprite[0]->begin_rendering();

}

void GameSceneUI::darw(){
	wasdSprite[0]->draw();

}
