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
	
}

void FailedUI::update() {}
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
	failedTex->draw();
	for (int i = 0; i < 7; i++) {
		failedLetter[i]->draw();
	}
}
