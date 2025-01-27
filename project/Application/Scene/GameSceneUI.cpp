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
	wasdSprite[4] = std::make_unique<SpriteInstance>("ResetUI.png");
	wasdSprite[5] = std::make_unique<SpriteInstance>("ESCkey.png");
	wasdSprite[6] = std::make_unique<SpriteInstance>("ReleseUI.png");
	tutorialUI = std::make_unique<SpriteInstance>("Tutorial1.png");

	for(int i = 0; i < uiIndex; i++)
	{
		wasdSprite[i]->get_transform().set_scale({ 0.5f, 1.0f });
		wasdSprite[i]->get_uv_transform().set_scale({ 0.5f, 1.0f });

	}
	wasdSprite[0]->get_transform().set_translate({ 76.2f,85 });
	wasdSprite[1]->get_transform().set_translate({ 7.3f,16 });
	wasdSprite[2]->get_transform().set_translate({ 76.2f,16 });
	wasdSprite[3]->get_transform().set_translate({ 143.7f,16 });
	wasdSprite[4]->get_transform().set_translate({ 10,218 });
	wasdSprite[5]->get_transform().set_translate({ 1141,16 });
	wasdSprite[6]->get_transform().set_translate({ 10,315 });

	tutorialUI->get_transform().set_translate({ 924,524 });


}

void GameSceneUI::update(){
	for(int i = 0; i < uiIndex; i++)
	{
		keyControl(i);
	}
	
	
}
#ifdef _DEBUG

#include <imgui.h>
void GameSceneUI::debugUpdate(){

	ImGui::Begin("sprite");
	tutorialUI->debug_gui();
	ImGui::End();

	/*for(int i = 0; i < uiIndex; i++)
	{

		ImGui::Begin(("sprite" + std::to_string(i)).c_str());
		wasdSprite[i]->debug_gui();
		ImGui::End();
	}*/
}
#endif
void GameSceneUI::begin_rendering(){
	for(int i = 0; i < uiIndex; i++)
	{
		wasdSprite[i]->begin_rendering();
	}

	tutorialUI->begin_rendering();

}

void GameSceneUI::darw(){
	for(int i = 0; i < uiIndex; i++)
	{
		wasdSprite[i]->draw();
	}
	tutorialUI->draw();
}

void GameSceneUI::keyControl(int index){
	KeyID keys[] = { KeyID::W, KeyID::A, KeyID::S, KeyID::D,KeyID::R,KeyID::Escape,KeyID::Space };

	if(Input::IsPressKey(keys[index])){
		wasdSprite[index]->get_uv_transform().set_translate_x(0.5f);
	} else {
		wasdSprite[index]->get_uv_transform().set_translate_x(0);
	}
}
