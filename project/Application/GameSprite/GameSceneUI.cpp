#include "GameSceneUI.h"

#include <algorithm>
#include <array>

#include <Library/Math/Definition.h>

#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Utility/Tools/SmartPointer.h>

GameSceneUI::GameSceneUI() = default;

GameSceneUI::~GameSceneUI() = default;

void GameSceneUI::initialize(int32_t level) {
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
	popUpUI = false;
	curEaseT = 0;
	wasdSprite[7]->get_transform().set_scale(CVector2::ZERO);

	stageFrame = eps::CreateUnique<SpriteInstance>("stageFrame.png", Vector2{ 0.5f, 0.5f });
	numberUi = eps::CreateUnique<SpriteInstance>("smallNumber.png", Vector2{ 0.5f, 0.5f });
	numberUi->get_transform().set_scale({ 0.1f,1.0f });
	numberUi->get_uv_transform().set_scale({ 0.1f,1.0f });
	numberUi10 = eps::CreateUnique<SpriteInstance>("smallNumber.png", Vector2{ 0.5f, 0.5f });
	numberUi10->get_transform().set_scale({ 0.1f,1.0f });
	numberUi10->get_uv_transform().set_scale({ 0.1f,1.0f });
	if (level < 10) {
		numberUi10->set_active(false);
	}
	numberUi->get_uv_transform().set_translate_x(level * 0.1f);
	numberUi10->get_uv_transform().set_translate_x((level / 10) * 0.1f);

	numCenter = { 128,640 - 40 };
	numberUi10->get_transform().set_translate({ numCenter.x - 59 / 2,numCenter.y });
	// 2桁表示
	if (level >= 10) {
		numberUi->get_transform().set_translate({ numCenter.x + 59 / 2,numCenter.y });
	}
	// 1桁表示
	else {
		numberUi->get_transform().set_translate(numCenter);
	}
	stageFrame->get_transform().set_translate(numCenter + Vector2{ 0,20 });
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

	stageFrame->begin_rendering();
	numberUi->begin_rendering();
	numberUi10->begin_rendering();
	tutorialUI->begin_rendering();

}

void GameSceneUI::darw() {
	for (int i = 0; i < uiIndex; i++) {
		wasdSprite[i]->draw();
	}
	tutorialUI->draw();
	stageFrame->draw();
	numberUi->draw();
	numberUi10->draw();
}

void GameSceneUI::ReleseUIUpdate() {
	if (curLevel <= 1) { return; }//ステージ３以降からUIを表示するため
	if (curEaseT > totalEaseT) { return; }

	if (!popUpUI) {
		popUpUI = isCanRelese;
	}
	if (popUpUI) {
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
	KeyID keys2[] = { KeyID::Up,KeyID::Left,KeyID::Down,KeyID::Right,KeyID::R,KeyID::Escape,KeyID::Z };
	constexpr std::array<PadID, 7> padTrigger = { PadID::Up, PadID::Left, PadID::Down, PadID::Right,PadID::Y,PadID::Start,PadID::B };
	constexpr std::array<Vector2, 4> stickDirection{
		CVector2::BACK,
		CVector2::FORWARD,
		CVector2::UP,
		CVector2::BACKWARD,
	};
	Vector2 stickL = Input::StickL().normalize_safe(1e-4f, CVector2::ZERO);
	bool stickInput = index < 4 ? Vector2::DotProduct(stickL, stickDirection[index]) < std::cos(PI_H) && stickL.length() != 0.0f : false;
	if (Input::IsPressKey(keys[index]) || Input::IsPressKey(keys2[index]) ||
		Input::IsPressPad(padTrigger[index]) || stickInput) {
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
