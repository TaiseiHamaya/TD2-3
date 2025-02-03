#include "FailedUI.h"
#include <Engine/Module/World/Sprite/SpriteInstance.h>

FailedUI::FailedUI() { init(); }

FailedUI::~FailedUI() {}

void FailedUI::init() {
	failedTex = std::make_unique<SpriteInstance>("Failed.png");
	failedLetter[0] = std::make_unique<SpriteInstance>("F.png", Vector2(0.5f, 0.5f));
	failedLetter[1] = std::make_unique<SpriteInstance>("a.png", Vector2(0.5f, 0.5f));
	failedLetter[2] = std::make_unique<SpriteInstance>("i.png", Vector2(0.5f, 0.5f));
	failedLetter[3] = std::make_unique<SpriteInstance>("l.png", Vector2(0.5f, 0.5f));
	failedLetter[4] = std::make_unique<SpriteInstance>("e.png", Vector2(0.5f, 0.5f));
	failedLetter[5] = std::make_unique<SpriteInstance>("d.png", Vector2(0.5f, 0.5f));
	failedLetter[6] = std::make_unique<SpriteInstance>("ten.png", Vector2(0.5f, 0.5f));

	endPos[0] = { 394.f,endPosHeight };
	endPos[1] = { 496.f,endPosHeight };
	endPos[2] = { 566.f,endPosHeight };
	endPos[3] = { 615.f,endPosHeight };
	endPos[4] = { 691.f,endPosHeight };
	endPos[5] = { 786.f,endPosHeight };
	endPos[6] = { 919.f,endPosHeight };
	
	for (int i = 0; i < 7; i++) {
		curEaseT[i] = 0;
		failedLetter[i]->get_transform().set_translate_x(endPos[i].x);
		failedLetter[i]->get_color().alpha = 0.f;
		startPos[i] = { 640-(3-i)*30.0f,720};
	}
	curDelayTime = 0;
	curIndex = 0;
}

void FailedUI::update() {

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

}
#ifdef _DEBUG

#include <imgui.h>
void FailedUI::debugUpdate() {

	for (int i = 0; i < 7; i++) {

		ImGui::Begin(("endPos" + std::to_string(i)).c_str());
		failedLetter[i]->debug_gui();
		ImGui::End();
	}
}
#endif
void FailedUI::begin_rendering() {
	failedTex->begin_rendering();
	for (int i = 0; i < 7; i++) {
		failedLetter[i]->begin_rendering();
	}
}

void FailedUI::draw() {
	//failedTex->draw();
	for (int i = 0; i < 7; i++) {
		failedLetter[i]->draw();
	}
}

void FailedUI::EaseChange(int index, float easeT) {

	float ratio = std::clamp(easeT / totalEaseT, 0.f, 1.f);
	//色
	//Easing::Out::Expo(ratio)
	failedLetter[index]->get_color().alpha = Easing::Out::Expo(ratio);
	//座標
	failedLetter[index]->get_transform().set_translate(
		{ std::lerp(
		startPos[index].x,
		endPos[index].x,
		Easing::Out::Quad(ratio)) ,std::lerp(
		startPos[index].y,
		endPos[index].y,
		Easing::Out::Quad(ratio)) }
	
	);
}
