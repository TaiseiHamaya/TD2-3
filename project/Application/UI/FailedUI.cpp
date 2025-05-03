#include "FailedUI.h"
#include <Engine/Module/World/Sprite/SpriteInstance.h>

#include <algorithm>

FailedUI::FailedUI() { init(); }

FailedUI::~FailedUI() {}

void FailedUI::init() {
	alignmentTex = std::make_unique<SpriteInstance>("Failed.png");
	letterTex[0] = std::make_unique<SpriteInstance>("F.png", Vector2(0.5f, 0.5f));
	letterTex[1] = std::make_unique<SpriteInstance>("a.png", Vector2(0.5f, 0.5f));
	letterTex[2] = std::make_unique<SpriteInstance>("i.png", Vector2(0.5f, 0.5f));
	letterTex[3] = std::make_unique<SpriteInstance>("l.png", Vector2(0.5f, 0.5f));
	letterTex[4] = std::make_unique<SpriteInstance>("e.png", Vector2(0.5f, 0.5f));
	letterTex[5] = std::make_unique<SpriteInstance>("d.png", Vector2(0.5f, 0.5f));
	letterTex[6] = std::make_unique<SpriteInstance>("ten.png", Vector2(0.5f, 0.5f));

	endPos[0] = { 394.f,endPosHeight };
	endPos[1] = { 496.f,endPosHeight };
	endPos[2] = { 566.f,endPosHeight };
	endPos[3] = { 615.f,endPosHeight };
	endPos[4] = { 691.f,endPosHeight };
	endPos[5] = { 786.f,endPosHeight };
	endPos[6] = { 919.f,endPosHeight };
	
	for (int i = 0; i < 7; i++) {
		curEaseT[i] = 0;
		letterTex[i]->get_transform().set_translate_x(endPos[i].x);
		letterTex[i]->get_color().alpha = 0.f;
		startPos[i] = { 640-(3-i)*30.0f,720};
	}
	curDelayTime = 0;
	curIndex = 0;

	failedReasonUI = std::make_unique<SpriteInstance>("FailedUI_1.png",Vector2(0.5f,0.5f));
	failedReasonUI->get_transform().set_scale({ 0.25f,1 });
	failedReasonUI->get_uv_transform().set_scale({ 0.25f,1 });
	failedReasonUI->get_transform().set_translate({ 640,265 });
	reaCurEaseT = 0;
	newScale = CVector2::ZERO;
	reaUpdateFlag = false;
	failedReasonUI->get_transform().set_scale(CVector2::ZERO);

	uiVisibleFlag = false;
	canOperation = false;

}

void FailedUI::update() {
	KeyID keys[] = { KeyID::Space, KeyID::R, KeyID::Z };
	PadID pad[] = { PadID::A, PadID::Y, PadID::B };
	for (int i = 0; i < 3; i++) {
		if (!canOperation && (Input::IsTriggerKey(keys[i]) || Input::IsTriggerPad(pad[i]))) {
			curIndex = 7;
			for (int i = 0; i < curIndex; i++) {
				curEaseT[i] = totalEaseT;
				//boundCurEaseT[i] = delayTotalTime * 6 - delayTotalTime * i;
			}
			reaCurEaseT = reaTotalEaseT;
			canOperation = true;
		}
	}
	
	if (curIndex < 7) {
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
	if (curEaseT[6] > totalEaseT) { reaUpdateFlag = true; }
	updateReason();
}
#ifdef _DEBUG

#include <imgui.h>
void FailedUI::debugUpdate() {

	/*for (int i = 0; i < 7; i++) {

		ImGui::Begin(("endPos" + std::to_string(i)).c_str());
		letterTex[i]->debug_gui();
		ImGui::End();
	}*/
}
#endif
void FailedUI::begin_rendering() {
	alignmentTex->begin_rendering();
	for (int i = 0; i < 7; i++) {
		letterTex[i]->begin_rendering();
	}
	failedReasonUI->begin_rendering();
}

void FailedUI::draw() {
	//failedTex->draw();
	for (int i = 0; i < 7; i++) {
		letterTex[i]->draw();
	}
	failedReasonUI->draw();

}

void FailedUI::EaseChange(int index, float easeT) {

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
		Easing::Out::Quad(ratio)) }
	
	);
}

void FailedUI::updateReason() {
	if (!reaUpdateFlag) { return; }
	reaCurEaseT += WorldClock::DeltaSeconds();
	float ratio = std::clamp(reaCurEaseT / reaTotalEaseT, 0.f, 1.f);

	newScale.x = 0.25f;//std::lerp(0.f, 0.25f, Easing::Out::Expo(ratio))
	newScale.y = std::lerp(0.f, 1.f, Easing::Out::Back(ratio));
	failedReasonUI->get_transform().set_scale(newScale);

	failedReasonUI->get_uv_transform().set_translate_x(0.25f * reasonIndex);

	if (ratio >= 1.0f) { uiVisibleFlag = true; }
}
