#include "GameManagement.h"

#include <algorithm>

#include <Library/Math/Transform2D.h>

#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/WorldClock/WorldClock.h>
#include <Engine/Utility/Tools/SmartPointer.h>


GameManagement::GameManagement() {
	operation = std::make_unique<AudioPlayer>();
	clearAudio = std::make_unique<AudioPlayer>();
	failedAudio = std::make_unique<AudioPlayer>();
	decision = std::make_unique<AudioPlayer>();
	resetAudio = std::make_unique<AudioPlayer>();
	backTitle = std::make_unique<AudioPlayer>();
	undoAudio = std::make_unique<AudioPlayer>();

	init();
}

GameManagement::~GameManagement() {}

void GameManagement::init() {
	clearFlag = false;
	failedFlag = false;
	isReset = false;
	isRestart = false;
	isTransition = false;
	isNext = false;
	isUndoRestart = false;
	canOperation = false;
	selectIndex = 1;//リトライ時に最初に選んでる方　0リトライ　1ネクスト、1手前からリスタート
	//clearSprite = std::make_unique<SpriteInstance>("ClearTex.png");
	//failedSprite = std::make_unique<SpriteInstance>("FailedTex.png");
	if (!resetBack) {
		resetBack = std::make_unique<SpriteInstance>("ResetBack.png", Vector2(0.5f, 0.5f));
		resetBack->get_transform().set_translate({ 640.0f, -360.0f });
	}
	if (!resetKoara) {
		resetKoara = std::make_unique<SpriteInstance>("KoaraFace.png", Vector2(0.5f, 0.5f));
		resetKoara->get_transform().set_translate({640.0f, -360.0f });
	}

	goSelect = std::make_unique<SpriteInstance>("GoSelect.png", Vector2(0.5f, 0.5f));
	goSelect->get_transform().set_scale({ 0.5f,1.0f });
	goSelect->get_uv_transform().set_scale({ 0.5f,1.0f });
	goSelect->get_transform().set_translate({ 789,247 });
	nextUI = std::make_unique<SpriteInstance>("Next.png", Vector2(0.5f, 0.5f));

	retryUI = std::make_unique<SpriteInstance>("Retry.png", Vector2(0.5f, 0.5f));
	nextUI->get_transform().set_scale({ 0.5f,1.0f });
	nextUI->get_uv_transform().set_scale({ 0.5f,1.0f });
	nextUI->get_transform().set_translate({ 789,247 });

	retryUI->get_transform().set_scale({ 0.5f,1.0f });
	retryUI->get_uv_transform().set_scale({ 0.5f,1.0f });

	selectFrame = std::make_unique<SpriteInstance>("SelectFrame.png", Vector2(0.5f, 0.5f));

	undoRetryUI = std::make_unique<SpriteInstance>("undoRetry.png", Vector2(0.5f, 0.5f));
	undoRetryUI->get_transform().set_scale({ 0.5f,1.0f });
	undoRetryUI->get_uv_transform().set_scale({ 0.5f,1.0f });
	undoRetryUI->get_transform().set_translate({ 789,169 });

	failedUI = std::make_unique<FailedUI>();
	clearUI = std::make_unique<ClearUI>();

	decision->initialize("decision.wav");
	operation->initialize("operation.wav");
	clearAudio->initialize("clearSound.wav");
	failedAudio->initialize("failedSound.wav");
	resultSoundFlag = false;
	clearFlag = false;
	failedFlag = false;

	resetAudio->initialize("reset.wav");
	backTitle->initialize("backAudio.wav");

	undoAudio->initialize("undo.wav");
}

void GameManagement::begin() {
	// 長押し対応
	if (Input::IsTriggerKey(KeyID::Escape) || Input::IsTriggerPad(PadID::Start)) {
		toSelectTimer = 10;
		backTitle->play();
	}
	else {
		toSelectTimer = 0;
	}
	// クリア、失敗状態ではない
	if (!(clearFlag || failedFlag || isTutorial)) {
		//if (Input::IsTriggerKey(KeyID::R) || Input::IsTriggerPad(PadID::Y)) {
		//	isReset = true;
		//	resetAudio->restart();
		//}
		reset_update();

	}
	else {
		//resultSoundFlagを使って効果音を鳴らす

		if (!resultSoundFlag) {
			if (clearFlag) {
				clearAudio->restart();
			}
			else if (failedFlag) {
				failedAudio->restart();
			}
			resultSoundFlag = true;
		}
		//toSelectTimer = 0;
		if ((Input::IsTriggerKey(KeyID::Space) || Input::IsTriggerPad(PadID::A)) && canOperation) {

			decision->restart();//確定の音
			isTransition = true;
			// カーソルがリトライを選んでる時
			if (selectIndex == 0) {
				isRestart = true;
				//clearFlag = false;
				//failedFlag = false;
			}
			// カーソルがネクストを選んでる時
			else {
				//スペースを押したら次のステージへ
				if (clearFlag) {
					isNext = true;
				}
				else if (failedFlag) {
					isUndoRestart = true;
				}
			}
		}
	}
}

void GameManagement::update() {

	resultKeyInput();
	selectFunc();
	if (failedFlag) {
		failedUI->update();
		canOperation = failedUI->GetCanOperation();
	}
	if (clearFlag) {
		clearUI->update();
		canOperation = clearUI->GetCanOperation();
	}

}
#ifdef _DEBUG

#include <imgui.h>
void GameManagement::debug_update() {

	ImGui::Begin("next");
	undoRetryUI->debug_gui();
	ImGui::End();
	failedUI->debugUpdate();
	clearUI->debugUpdate();

	ImGui::Begin("Reset");
	resetBack->debug_gui();
	resetKoara->debug_gui();
	ImGui::End();
}
#endif
void GameManagement::begin_rendering() {
	//clearSprite->begin_rendering();
	failedUI->begin_rendering();
	clearUI->begin_rendering();
	nextUI->begin_rendering();
	retryUI->begin_rendering();
	selectFrame->begin_rendering();
	undoRetryUI->begin_rendering();
	goSelect->begin_rendering();
	resetBack->begin_rendering();
	resetKoara->begin_rendering();
}

void GameManagement::darw() {
	if (clearFlag) {
		clearUI->draw();

		if (clearUI->GetUiVisible()) {
			if (curLevel >= maxLevel) {
				goSelect->draw();
			}
			else {
				nextUI->draw();

			}
			retryUI->draw();
			selectFrame->draw();
		}

	}
	else if (failedFlag) {
		failedUI->draw();
		if (failedUI->GetUiVisible()) {
			retryUI->draw();
			selectFrame->draw();
			undoRetryUI->draw();
		}

	}
	resetBack->draw();
	resetKoara->draw();
}

void GameManagement::resultKeyInput() {
	if (!canOperation) { return; }
	if (!clearFlag && !failedFlag) { return; }
	//if (Input::IsTriggerKey(KeyID::R) || Input::IsTriggerPad(PadID::Y)) {
	//	isReset = true;
	//	resetAudio->restart();
	//}
	reset_update();
	if (Input::IsTriggerKey(KeyID::Z) || Input::IsTriggerPad(PadID::B)) {
		isUndoRestart = true;
		undoAudio->restart();
	}
}

void GameManagement::selectFunc() {
	if (!clearFlag && !failedFlag) {
		return;
	}
	Vector2 stickL = Input::StickL();
	if (Input::IsTriggerKey(KeyID::A) || Input::IsTriggerKey(KeyID::Left) || Input::IsTriggerPad(PadID::Left) || stickL.x < 0) {
		selectIndex--;
		operation->restart();//選択時の音
	}
	if (Input::IsTriggerKey(KeyID::D) || Input::IsTriggerKey(KeyID::Right) || Input::IsTriggerPad(PadID::Right) || stickL.x > 0) {
		selectIndex++;
		operation->restart();//選択時の音
	}
	selectIndex = std::clamp(selectIndex, 0, 1);

	if (failedFlag) {
		//selectIndex = 0;
		retryUI->get_transform().set_translate({ 458,169 });

	}
	else if (clearFlag) {


		retryUI->get_transform().set_translate({ 458,247 });

	}

	if (selectIndex == 0) {
		retryUI->get_uv_transform().set_translate_x(0);
		nextUI->get_uv_transform().set_translate_x(0.5f);
		goSelect->get_uv_transform().set_translate_x(0.5f);
		undoRetryUI->get_uv_transform().set_translate_x(0.5f);


		selectFrame->get_transform().set_translate(retryUI->get_transform().get_translate());
	}
	else {
		retryUI->get_uv_transform().set_translate_x(0.5f);
		nextUI->get_uv_transform().set_translate_x(0);
		goSelect->get_uv_transform().set_translate_x(0);
		undoRetryUI->get_uv_transform().set_translate_x(0);

		//選択中のUIの位置にフレームを移動
		if (clearFlag) {
			selectFrame->get_transform().set_translate(nextUI->get_transform().get_translate());

		}
		else if (failedFlag) {
			selectFrame->get_transform().set_translate(undoRetryUI->get_transform().get_translate());

		}

	}

}

void GameManagement::reset_update() {
	float t = 0.0f;
	float newPos = 0.0f;
	float koaraAngle = 0.0f;
	switch (resetState) {
	case ResetState::Idle:
		// 今の時間を0にしておく
		if (resetCurrentTime < 0.0f) {
			resetCurrentTime = 0.0f;
		}
		else {
			resetCurrentTime -= WorldClock::DeltaSeconds();
		}

		t = std::clamp(resetCurrentTime / resetMaxTime, 0.0f, 1.0f);

		newPos = -360.0f + (360.0f + 360.0f) * t;
		resetBack->get_transform().set_translate_y(newPos);
		resetKoara->get_transform().set_translate_y(newPos);

		if (Input::IsTriggerKey(KeyID::R) || Input::IsTriggerPad(PadID::Y)) {
			resetState = ResetState::Pressing;
		}
		break;
	case ResetState::Pressing:
		resetCurrentTime += WorldClock::DeltaSeconds();

		t = std::clamp(resetCurrentTime / resetMaxTime, 0.0f, 1.0f);

		newPos = -360.0f + (360.0f + 360.0f) * t;
		resetBack->get_transform().set_translate_y(newPos);
		resetKoara->get_transform().set_translate_y(newPos);

		if (resetCurrentTime >= resetMaxTime) {
			resetState = ResetState::Resetting;
			resetAudio->restart();
			resetCurrentTime = 0.0f;
		}

		if (!Input::IsPressKey(KeyID::R) && !Input::IsPressPad(PadID::Y)) {
			resetState = ResetState::Idle;
		}
		break;
	case ResetState::Resetting:
		resetCurrentTime += WorldClock::DeltaSeconds();
		t = std::clamp(resetCurrentTime / resetMaxTime, 0.0f, 1.0f);

		koaraAngle = 0.8f * std::exp(-0.6f * t) * std::sin(2 * 3.14159f * 1.5f * t);
		resetKoara->get_transform().set_rotate(koaraAngle);

		if (resetCurrentTime >= resetMaxTime) {
			resetState = ResetState::Completed;
			resetCurrentTime = 0.0f;
			isReset = true;
		}
		break;
	case ResetState::Completed:
		resetCurrentTime += WorldClock::DeltaSeconds();

		t = std::clamp(resetCurrentTime / resetMaxTime, 0.0f, 1.0f);
		newPos = 360.0f + (1080.0f + -360.0f) * t;
		resetBack->get_transform().set_translate_y(newPos);
		resetKoara->get_transform().set_translate_y(newPos);


		if (resetCurrentTime >= resetMaxTime) {
			resetState = ResetState::Idle;
			resetCurrentTime = 0.0f;
		}

		break;
	}


}
