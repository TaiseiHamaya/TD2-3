#include "ClearUI.h"
#include <Engine/Module/World/Sprite/SpriteInstance.h>

#include <algorithm>

ClearUI::ClearUI() { init(); }

ClearUI::~ClearUI() {}

void ClearUI::init() {
	alignmentTex = std::make_unique<SpriteInstance>("Clear.png");
	letterTex[0] = std::make_unique<SpriteInstance>("C.png", Vector2(0.5f, 0.5f));
	letterTex[1] = std::make_unique<SpriteInstance>("L.png", Vector2(0.5f, 0.5f));
	letterTex[2] = std::make_unique<SpriteInstance>("E.png", Vector2(0.5f, 0.5f));
	letterTex[3] = std::make_unique<SpriteInstance>("A.png", Vector2(0.5f, 0.5f));
	letterTex[4] = std::make_unique<SpriteInstance>("R.png", Vector2(0.5f, 0.5f));
	letterTex[5] = std::make_unique<SpriteInstance>("!.png", Vector2(0.5f, 0.5f));

	endPos[0] = { 376.f,endPosHeight };
	endPos[1] = { 477.f,endPosHeight };
	endPos[2] = { 594.f,endPosHeight };
	endPos[3] = { 703.f,endPosHeight };
	endPos[4] = { 827.f,endPosHeight };
	endPos[5] = { 922.f,endPosHeight };
	for (int i = 0; i < 6; i++) {
		curEaseT[i] = 0;
		letterTex[i]->get_transform().set_translate_x(endPos[i].x);
		letterTex[i]->get_color().alpha = 0.f;
		startPos[i] = { 640 - (3 - i) * 200.0f,0 };
		boundCurEaseT[i] = 0;
		boundEaseDir[i] = 1;
		//letterTex[i]->get_transform().set_scale({ 1.2f,0.8f });
	}
	curDelayTime = 0;
	curIndex = 0;
	canOperation = false;
}

void ClearUI::update() {

	if (!canOperation && Input::IsTriggerKey(KeyID::Space)) {
		curIndex = 6;
		for (int i = 0; i < curIndex; i++) {
			curEaseT[i] = totalEaseT;
			boundCurEaseT[i] = delayTotalTime * 6 - delayTotalTime * i;
		}
		canOperation = true;
	}
	if (curIndex < 6) {
		curDelayTime += WorldClock::DeltaSeconds();
		if (curDelayTime >= delayTotalTime) {
			curDelayTime = 0;
			curIndex++;
		}
	}
	for (int i = 0; i < curIndex; i++) {
		curEaseT[i] += WorldClock::DeltaSeconds();
		EaseChange(i, curEaseT[i]);
	}
	for (int i = 0; i < 6; i++) {
		BoundEase(i);
	}
	if (curEaseT[5] > totalEaseT) { uiVisibleFlag = true; }
}
#ifdef _DEBUG

#include <imgui.h>
void ClearUI::debugUpdate() {
	/*for (int i = 0; i < 6; i++) {

		ImGui::Begin(("endPos" + std::to_string(i)).c_str());
		letterTex[i]->debug_gui();
		ImGui::End();
	}*/
}
#endif
void ClearUI::begin_rendering() {
	alignmentTex->begin_rendering();
	for (int i = 0; i < 6; i++) {
		letterTex[i]->begin_rendering();
	}
}

void ClearUI::draw() {
	//alignmentTex->draw();
	for (int i = 0; i < 6; i++) {
		letterTex[i]->draw();
	}
}

void ClearUI::EaseChange(int index, float easeT) {

	float ratio = std::clamp(easeT / totalEaseT, 0.f, 1.f);
	//色
	//Easing::Out::Expo(ratio)
	letterTex[index]->get_color().alpha = Easing::Out::Expo(ratio);
	//座標
	letterTex[index]->get_transform().set_translate(
		{ std::lerp(
		startPos[index].x,
		endPos[index].x,
		Easing::Out::Quad(ratio)) ,std::lerp(
		startPos[index].y,
		endPos[index].y,
		Easing::Out::Back(ratio)) }

	);
}

void ClearUI::BoundEase(int index) {
	if (curEaseT[index] < totalEaseT) { return; }
	if (curEaseT[5] >= totalEaseT) { canOperation = true; }
	boundCurEaseT[index] += WorldClock::DeltaSeconds() * boundEaseDir[index];

	if (boundCurEaseT[index] > boundTotalEaseT) {
		boundCurEaseT[index] = boundTotalEaseT;
		boundEaseDir[index] *= -1;
	}
	else if (boundCurEaseT[index] < 0) {
		boundCurEaseT[index] = 0;
		boundEaseDir[index] *= -1;
	}

	float ratio = std::clamp(boundCurEaseT[index] / boundTotalEaseT, 0.f, 1.f);

	letterTex[index]->get_transform().set_translate_y(
		std::lerp(
			endPosHeight,
			endPosHeight + boundLength,
			Easing::Out::Sine(ratio))

	);
	Vector2 newScale = CVector2::ZERO;
	newScale.x = std::lerp(1.1f, 0.8f, Easing::Out::Sine(ratio));
	newScale.y = std::lerp(0.9f, 1.2f, Easing::Out::Sine(ratio));
	letterTex[index]->get_transform().set_scale(newScale);
}
