#include "Application/Scene/GameManagement.h"

#include <algorithm>

#include <Library/Math/Transform2D.h>

#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/WorldClock/WorldClock.h>

GameManagement::GameManagement() {
	init();
}

GameManagement::~GameManagement() {}

void GameManagement::init() {
	clearFlag = false;
	failedFlag = false;
	isReset = false;
	isNext = false;
	clearSprite = std::make_unique<SpriteInstance>("ClearTex.png");
	failedSprite = std::make_unique<SpriteInstance>("FailedTex.png");

	nextUI = std::make_unique<SpriteInstance>("Next.png");
	retryUI = std::make_unique<SpriteInstance>("Retry.png");
	nextUI->get_transform().set_scale({ 0.5f, 1.0f });
	nextUI->get_uv_transform().set_scale({ 0.5f, 1.0f });
	retryUI->get_transform().set_scale({ 0.5f, 1.0f });
	retryUI->get_uv_transform().set_scale({ 0.5f, 1.0f });

	selectFrame = std::make_unique<SpriteInstance>("SelectFrame.png");
	failedUI = std::make_unique<SpriteInstance>("FailedUI_1.png");
	failedUI->get_transform().set_scale({ 0.25f,1 });
	failedUI->get_uv_transform().set_scale({ 0.25f,1 });
	failedUI->get_transform().set_translate({ 257,220 });

	decision = std::make_unique<AudioPlayer>();
	decision->initialize("decision.wav");
	operation = std::make_unique<AudioPlayer>();
	operation->initialize("operation.wav");
}

void GameManagement::begin() {
	// クリア、失敗状態ではない
	if (!(clearFlag || failedFlag)) {
		isReset = Input::IsTriggerKey(KeyID::R);
		// 長押し対応
		if (Input::IsPressKey(KeyID::Escape)) {
			toSelectTimer += WorldClock::DeltaSeconds();
		}
		else {
			toSelectTimer = 0;
		}
	}
	else {
		toSelectTimer = 0;
		if (Input::IsTriggerKey(KeyID::Space)) {

			decision->restart();//確定の音
			// カーソルがリトライを選んでる時
			if (selectIndex == 0) {
				isReset = true;
				clearFlag = false;
				failedFlag = false;
			}
			// カーソルがネクストを選んでる時
			else
			{
				//スペースを押したら次のステージへ
				isNext = true;
				clearFlag = false;
				failedFlag = false;
			}

		}
	}
}

void GameManagement::update() {

	selectFunc();

}
#ifdef _DEBUG

#include <imgui.h>
void GameManagement::debug_update() {

	ImGui::Begin("next");
	failedUI->debug_gui();
	ImGui::End();
}
#endif
void GameManagement::begin_rendering() {
	clearSprite->begin_rendering();
	failedSprite->begin_rendering();
	nextUI->begin_rendering();
	retryUI->begin_rendering();
	selectFrame->begin_rendering();
	failedUI->begin_rendering();

}

void GameManagement::darw() {
	if (clearFlag) {
		clearSprite->draw();

		nextUI->draw();
		retryUI->draw();
		selectFrame->draw();
	}
	else 	if (failedFlag) {
		failedSprite->draw();
		retryUI->draw();
		selectFrame->draw();
		failedUI->draw();
	}
}

void GameManagement::selectFunc() {
	if (!clearFlag && !failedFlag) { return; }

	if (failedFlag) {
		selectIndex = 0;
		retryUI->get_transform().set_translate({ 485,137 });
		failedUI->get_uv_transform().set_translate_x(0.25f * failedSelectIndex);

	}
	else if (clearFlag) {

		if (Input::IsTriggerKey(KeyID::A)) {
			selectIndex--;
			operation->restart();//選択時の音
		}
		if (Input::IsTriggerKey(KeyID::D)) {
			selectIndex++;
			operation->restart();//選択時の音
		}
		selectIndex = std::clamp(selectIndex, 0, 1);

		retryUI->get_transform().set_translate({ 322,215 });
		nextUI->get_transform().set_translate({ 653,215 });

	}

	if (selectIndex == 0) {
		retryUI->get_uv_transform().set_translate_x(0);
		nextUI->get_uv_transform().set_translate_x(0.5f);
		selectFrame->get_transform().set_translate({ 322 - 16, 215 - 16 });



	}
	else {
		retryUI->get_uv_transform().set_translate_x(0.5f);
		nextUI->get_uv_transform().set_translate_x(0);
		selectFrame->get_transform().set_translate({ 653 - 16, 215 - 16 });

	}

	if (failedFlag) {
		selectFrame->get_transform().set_translate({ 485 - 16, 137 - 16 });
	}

}
