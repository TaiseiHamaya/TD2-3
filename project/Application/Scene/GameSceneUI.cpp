#include "GameSceneUI.h"
#include "Engine/Module/World/Sprite/SpriteInstance.h"
#include <Engine/Runtime/Input/Input.h>

GameSceneUI::GameSceneUI(){ init(); }

GameSceneUI::~GameSceneUI(){}

void GameSceneUI::init(){
	wasdSprite[0] = std::make_unique<SpriteInstance>("Wkey.png");
	wasdSprite[1] = std::make_unique<SpriteInstance>("Akey.png");
	wasdSprite[2] = std::make_unique<SpriteInstance>("Skey.png");
	wasdSprite[3] = std::make_unique<SpriteInstance>("Dkey.png");

	for(int i = 0; i < 4; i++)
	{
		wasdSprite[i]->get_transform().set_scale({ 0.5f, 1.0f });
		wasdSprite[i]->get_uv_transform().set_scale({ 0.5f, 1.0f });
		
	}
	wasdSprite[0]->get_transform().set_translate({ 76.2f,133.4f });
	wasdSprite[1]->get_transform().set_translate({ 7.3f,64.0f });
	wasdSprite[2]->get_transform().set_translate({ 76.2f,64.0f });
	wasdSprite[3]->get_transform().set_translate({ 143.7f,64.0f });

}
#ifdef _DEBUG

#include <imgui.h>
void GameSceneUI::update(){
	for(int i = 0; i < 4; i++)
	{
		keyControl(i);
	}
	/*for(int i = 0; i < 4; i++)
	{
		
		ImGui::Begin(("sprite" + std::to_string(i)).c_str());
		wasdSprite[i]->debug_gui();
		ImGui::End();
	}*/
}
#endif
void GameSceneUI::begin_rendering(){
	for(int i = 0; i < 4; i++)
	{
		wasdSprite[i]->begin_rendering();
	}
	
	

}

void GameSceneUI::darw(){
	for(int i = 0; i < 4; i++)
	{
		wasdSprite[i]->draw();
	}
}

void GameSceneUI::keyControl( int index){
	KeyID keys[] = { KeyID::W, KeyID::A, KeyID::S, KeyID::D };

	if(Input::IsPressKey(keys[index])){
		wasdSprite[index]->get_uv_transform().set_translate_x(0.5f);
	} else {
		wasdSprite[index]->get_uv_transform().set_translate_x(0);
	}
}
