#include "GameSceneUI.h"
#include "Engine/Module/World/Sprite/SpriteInstance.h"
#include <Engine/Runtime/Input/Input.h>
#include <algorithm>

GameSceneUI::GameSceneUI() { init(); }

GameSceneUI::~GameSceneUI() {}

void GameSceneUI::init() {
	wasdSprite[0] = std::make_unique<SpriteInstance>("Wkey.png");
	wasdSprite[1] = std::make_unique<SpriteInstance>("Akey.png");
	wasdSprite[2] = std::make_unique<SpriteInstance>("Skey.png");
	wasdSprite[3] = std::make_unique<SpriteInstance>("Dkey.png");
	wasdSprite[4] = std::make_unique<SpriteInstance>("ResetUI.png");
	wasdSprite[5] = std::make_unique<SpriteInstance>("ESCkey.png");
	wasdSprite[6] = std::make_unique<SpriteInstance>("Undo.png");
	wasdSprite[7] = std::make_unique<SpriteInstance>("ReleseUI.png", Vector2(0.5f, 0.5f));

	tutorialUI = std::make_unique<SpriteInstance>("Tutorial1.png");

	for (int i = 0; i < uiIndex; i++) {
		wasdSprite[i]->get_transform().set_scale({ 0.5f, 1.0f });
		wasdSprite[i]->get_uv_transform().set_scale({ 0.5f, 1.0f });

	}
	wasdSprite[0]->get_transform().set_translate({ 106.2f,103 });
	wasdSprite[1]->get_transform().set_translate({ 37.3f,30 });
	wasdSprite[2]->get_transform().set_translate({ 106.2f,30 });
	wasdSprite[3]->get_transform().set_translate({ 173.7f,30 });
	wasdSprite[4]->get_transform().set_translate({ 30,218 });
	wasdSprite[5]->get_transform().set_translate({ 1141,30 });
	wasdSprite[6]->get_transform().set_translate({ 171,218 });
	wasdSprite[7]->get_transform().set_translate({ 138.2f,379 });

	tutorialUI->get_transform().set_translate({ 924,524 });

	isCanRelese = false;
	curEaseT = 0;
	wasdSprite[7]->get_transform().set_scale(CVector2::ZERO);

}

void GameSceneUI::update() {
	for (int i = 0; i < uiIndex - 1; i++) {
		keyControl(i);
	}
	ReleseUIUpdate();
	wasdSprite[7]->get_uv_transform().set_translate_x(0.5f * !isCanRelese);

}
#ifdef _DEBUG

#include <imgui.h>
void GameSceneUI::debugUpdate() {

	ImGui::Begin("sprite");
	wasdSprite[6]->debug_gui();
	ImGui::End();
	ImGui::Begin("debug");
	ImGui::Text("ratio=%f", ratio);
	ImGui::End();

	/*for(int i = 0; i < uiIndex; i++)
	{

		ImGui::Begin(("sprite" + std::to_string(i)).c_str());
		wasdSprite[i]->debug_gui();
		ImGui::End();
	}*/
}
#endif
void GameSceneUI::begin_rendering() {
	for (int i = 0; i < uiIndex; i++) {
		wasdSprite[i]->begin_rendering();
	}

	tutorialUI->begin_rendering();

}

void GameSceneUI::darw() {
	for (int i = 0; i < uiIndex; i++) {
		wasdSprite[i]->draw();
	}
	tutorialUI->draw();
}

void GameSceneUI::ReleseUIUpdate() {
	if (curLevel <= 2) { return; }//ステージ３以降からUIを表示するため
	if (curEaseT > totalEaseT) { return; }
	if (isCanRelese) {
		curEaseT += WorldClock::DeltaSeconds();
		curEaseT = std::clamp(curEaseT, 0.f, totalEaseT);

		Vector2 newScale = CVector2::ZERO;
		 ratio = curEaseT / totalEaseT;
		 newScale.x = OutBack(ratio, 1, 0, 0.5f, 3);
		newScale.y = OutBack(ratio, 1, 0, 1.f, 3);
		wasdSprite[7]->get_transform().set_scale(newScale);
	}
}

void GameSceneUI::keyControl(int index) {
	KeyID keys[] = { KeyID::W, KeyID::A, KeyID::S, KeyID::D,KeyID::R,KeyID::Escape,KeyID::Z };

	if (Input::IsPressKey(keys[index])) {
		wasdSprite[index]->get_uv_transform().set_translate_x(0.5f);
	}
	else {
		wasdSprite[index]->get_uv_transform().set_translate_x(0);
	}
}

float GameSceneUI::OutBack(float t, float totaltime, float min, float max, float s) {
	s *= 1.70158f;
	max -= min;
	t /= totaltime;

	t--;


	return max * (t * t * ((s + 1) * t + s) + 1) + min;
}
