#include "GameSceneUI.h"

#include <algorithm>
#include <array>

#include <Library/Math/Definition.h>
#include <Library/Math/Transform2D.h>
#include <Library/Utility/Tools/SmartPointer.h>

#include <Engine/Debug/ImGui/ImGuiLoadManager/ImGuiLoadManager.h>
#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Runtime/Clock/WorldClock.h>
#include <Engine/Runtime/Input/Input.h>

#include "Application/GameValue.h"
#include <Application/Configuration/Configuration.h>

GameSceneUI::GameSceneUI() = default;

GameSceneUI::~GameSceneUI() = default;

void GameSceneUI::initialize(int32_t level) {
	controlSprite[0][0] = std::make_unique<SpriteInstance>("Wkey.png");
	controlSprite[0][1] = std::make_unique<SpriteInstance>("Akey.png");
	controlSprite[0][2] = std::make_unique<SpriteInstance>("Skey.png");
	controlSprite[0][3] = std::make_unique<SpriteInstance>("Dkey.png");
	controlSprite[0][4] = std::make_unique<SpriteInstance>("ResetUIController.png");
	controlSprite[0][5] = std::make_unique<SpriteInstance>("ESCkeyController.png");
	controlSprite[0][6] = std::make_unique<SpriteInstance>("UndoController.png");
	switch (Configuration::GetLanguage()) {
	case Configuration::Language::Japanese:
		controlSprite[0][7] = std::make_unique<SpriteInstance>("ReleseUIController.png", Vector2(0.5f, 0.5f));
		break;
	case Configuration::Language::English:
		controlSprite[0][7] = std::make_unique<SpriteInstance>("ReleseUIController_EN.png", Vector2(0.5f, 0.5f));
		break;
	}

	controlSprite[1][0] = std::make_unique<SpriteInstance>("Wkey.png");
	controlSprite[1][1] = std::make_unique<SpriteInstance>("Akey.png");
	controlSprite[1][2] = std::make_unique<SpriteInstance>("Skey.png");
	controlSprite[1][3] = std::make_unique<SpriteInstance>("Dkey.png");
	controlSprite[1][4] = std::make_unique<SpriteInstance>("ResetUI.png");
	controlSprite[1][5] = std::make_unique<SpriteInstance>("ESCkey.png");
	controlSprite[1][6] = std::make_unique<SpriteInstance>("Undo.png");
	switch (Configuration::GetLanguage()) {
	case Configuration::Language::Japanese:
		controlSprite[1][7] = std::make_unique<SpriteInstance>("ReleseUI.png", Vector2(0.5f, 0.5f));
		break;
	case Configuration::Language::English:
		controlSprite[1][7] = std::make_unique<SpriteInstance>("ReleseUI_EN.png", Vector2(0.5f, 0.5f));
		break;
	}

	noneButtonSprite = std::make_unique<SpriteInstance>("NoneButton.png");

	tutorialUI = std::make_unique<SpriteInstance>("Tutorial1.png");

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < uiIndex; j++) {
			controlSprite[i][j]->get_transform().set_scale({ 0.5f, 1.0f });
			controlSprite[i][j]->get_uv_transform().set_scale({ 0.5f, 1.0f });
		}
	}
	controlSprite[0][0]->get_transform().set_translate({ 106.2f,176 });
	controlSprite[0][1]->get_transform().set_translate({ 37.3f,103 });
	controlSprite[0][2]->get_transform().set_translate({ 106.2f,30 });
	controlSprite[0][3]->get_transform().set_translate({ 173.7f,103 });
	controlSprite[0][4]->get_transform().set_translate({ 115,453 });
	controlSprite[0][5]->get_transform().set_translate({ 1141,30 });
	controlSprite[0][6]->get_transform().set_translate({ 171,379 });
	controlSprite[0][7]->get_transform().set_translate({ 147,307 });

	controlSprite[1][0]->get_transform().set_translate({ 106.2f,103 });
	controlSprite[1][1]->get_transform().set_translate({ 37.3f,30 });
	controlSprite[1][2]->get_transform().set_translate({ 106.2f,30 });
	controlSprite[1][3]->get_transform().set_translate({ 173.7f,30 });
	controlSprite[1][4]->get_transform().set_translate({ 30,218 });
	controlSprite[1][5]->get_transform().set_translate({ 1141,30 });
	controlSprite[1][6]->get_transform().set_translate({ 171,218 });
	controlSprite[1][7]->get_transform().set_translate({ 138.2f,379 });

	noneButtonSprite->get_transform().set_translate({ 56,379 });

	tutorialUI->get_transform().set_translate({ 924,524 });

	isCanRelese = false;
	popUpUI = false;
	curEaseT = 0;
	controlSprite[0][7]->get_transform().set_scale(CVector2::ZERO);
	controlSprite[1][7]->get_transform().set_scale(CVector2::ZERO);

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
	InputType controlType = GameValue::UiType.get_type();
	for (int i = 0; i < uiIndex - 1; i++) {
		if (controlType == InputType::Pad) {
			padControl(i);
		}
		else {
			keyControl(i);
		}
	}
	ReleseUIUpdate();
	controlSprite[0][7]->get_uv_transform().set_translate_x(0.5f * !isCanRelese);
	controlSprite[1][7]->get_uv_transform().set_translate_x(0.5f * !isCanRelese);
}
#ifdef _DEBUG

#include <imgui.h>
void GameSceneUI::debugUpdate() {

	ImGui::Begin("sprite");
	controlSprite[0][6]->debug_gui();
	ImGui::End();
	ImGui::Begin("debug");
	ImGui::Text("ratio=%f", ratio);
	ImGui::End();
	ImGui::Begin(("noneButtonSprite"));
	noneButtonSprite->debug_gui();
	ImGui::End();

}
#endif
void GameSceneUI::begin_rendering() {
	int controlType = (int)GameValue::UiType.get_type();
	for (int i = 0; i < uiIndex; i++) {
		controlSprite[controlType][i]->begin_rendering();
	}

	noneButtonSprite->begin_rendering();
	stageFrame->begin_rendering();
	numberUi->begin_rendering();
	numberUi10->begin_rendering();
	tutorialUI->begin_rendering();

}

void GameSceneUI::darw() {
	int controlType = (int)GameValue::UiType.get_type();
	for (int i = 0; i < uiIndex; i++) {
		controlSprite[controlType][i]->draw();
	}
	if (GameValue::UiType.get_type() == InputType::Pad) {
		noneButtonSprite->draw();
	}
	tutorialUI->draw();
	stageFrame->draw();
	numberUi->draw();
	numberUi10->draw();
}

void GameSceneUI::ReleseUIUpdate() {
	//if (curLevel <= 1) { return; }//ステージ３以降からUIを表示するため
	if (curEaseT > totalEaseT) { return; }

	int controlType = (int)GameValue::UiType.get_type();
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
		controlSprite[0][7]->get_transform().set_scale(newScale);
		controlSprite[1][7]->get_transform().set_scale(newScale);
	}
}

void GameSceneUI::keyControl(int index) {
	KeyID keys[] = { KeyID::W, KeyID::A, KeyID::S, KeyID::D,KeyID::R,KeyID::Escape,KeyID::Z };
	KeyID keys2[] = { KeyID::Up,KeyID::Left,KeyID::Down,KeyID::Right,KeyID::R,KeyID::Escape,KeyID::Z };
	if (Input::IsPressKey(keys[index]) || Input::IsPressKey(keys2[index])) {
		controlSprite[1][index]->get_uv_transform().set_translate_x(0.5f);
	}
	else {
		controlSprite[1][index]->get_uv_transform().set_translate_x(0);
	}
}

void GameSceneUI::padControl(int index) {
	constexpr std::array<PadID, 7> padTrigger = { PadID::Up, PadID::Left, PadID::Down, PadID::Right,PadID::Y,PadID::Start,PadID::B };
	constexpr std::array<Vector2, 4> stickDirection{
		CVector2::UP,
		CVector2::BACKWARD,
		CVector2::DOWN,
		CVector2::FORWARD,
	};
	Vector2 stickL = Input::StickL().normalize_safe(CVector2::ZERO);
	bool stickInput = index < 4 ? Vector2::Dot(stickL, stickDirection[index]) > std::cos(PI / 4) && stickL.length() != 0.0f : false;
	if (Input::IsPressPad(padTrigger[index]) || stickInput) {
		controlSprite[0][index]->get_uv_transform().set_translate_x(0.5f);
	}
	else {
		controlSprite[0][index]->get_uv_transform().set_translate_x(0);
	}
}

float GameSceneUI::OutBack(float t, float totaltime, float min, float max, float s) {
	s *= 1.70158f;
	max -= min;
	t /= totaltime;

	t--;


	return max * (t * t * ((s + 1) * t + s) + 1) + min;
}
