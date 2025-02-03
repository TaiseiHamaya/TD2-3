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
	selectIndex = 1;//リトライ時に最初に選んでる方　0リトライ　1ネクスト、1手前からリスタート
	//clearSprite = std::make_unique<SpriteInstance>("ClearTex.png");
	//failedSprite = std::make_unique<SpriteInstance>("FailedTex.png");


	goSelect = std::make_unique<SpriteInstance>("GoSelect.png",Vector2(0.5f,0.5f));
	goSelect->get_transform().set_scale({0.5f,1.0f});
	goSelect->get_uv_transform().set_scale({0.5f,1.0f});
	goSelect->get_transform().set_translate({789,247});
	nextUI = std::make_unique<SpriteInstance>("Next.png",Vector2(0.5f,0.5f));

	retryUI = std::make_unique<SpriteInstance>("Retry.png",Vector2(0.5f,0.5f));
	nextUI->get_transform().set_scale({0.5f,1.0f});
	nextUI->get_uv_transform().set_scale({0.5f,1.0f});
	nextUI->get_transform().set_translate({789,247});

	retryUI->get_transform().set_scale({0.5f,1.0f});
	retryUI->get_uv_transform().set_scale({0.5f,1.0f});

	selectFrame = std::make_unique<SpriteInstance>("SelectFrame.png",Vector2(0.5f,0.5f));

	undoRetryUI= std::make_unique<SpriteInstance>("undoRetry.png",Vector2(0.5f,0.5f));
	undoRetryUI->get_transform().set_scale({0.5f,1.0f});
	undoRetryUI->get_uv_transform().set_scale({0.5f,1.0f});
	undoRetryUI->get_transform().set_translate({789,169});

	failedUI = std::make_unique<FailedUI>();
	clearUI = std::make_unique<ClearUI>();

	decision = std::make_unique<AudioPlayer>();
	decision->initialize("decision.wav");
	operation = std::make_unique<AudioPlayer>();
	operation->initialize("operation.wav");
	clearAudio = std::make_unique<AudioPlayer>();
	clearAudio->initialize("clearSound.wav");
	failedAudio = std::make_unique<AudioPlayer>();
	failedAudio->initialize("failedSound.wav");
	resultSoundFlag = false;
}

void GameManagement::begin() {
	// クリア、失敗状態ではない
	if(!(clearFlag || failedFlag)) {
		isReset = Input::IsTriggerKey(KeyID::R);
		// 長押し対応
		if(Input::IsPressKey(KeyID::Escape)) {
			toSelectTimer += WorldClock::DeltaSeconds();
		} else {
			toSelectTimer = 0;
		}

	} else {
		//resultSoundFlagを使って効果音を鳴らす

		if(!resultSoundFlag) {
			if(clearFlag) {
				clearAudio->restart();
			} else if(failedFlag) {
				failedAudio->restart();
			}
			resultSoundFlag = true;
		}
		toSelectTimer = 0;
		if(Input::IsTriggerKey(KeyID::Space)) {

			decision->restart();//確定の音
			// カーソルがリトライを選んでる時
			if(selectIndex == 0) {
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
	if (failedFlag) { failedUI->update(); }
	if (clearFlag) { clearUI->update(); }
	
}
#ifdef _DEBUG

#include <imgui.h>
void GameManagement::debug_update() {

	ImGui::Begin("next");
	undoRetryUI->debug_gui();
	ImGui::End();
	failedUI->debugUpdate();
	clearUI->debugUpdate();
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

}

void GameManagement::darw() {
	if(clearFlag) {
		clearUI->draw();

		if(curLevel>=maxLevel){
			goSelect->draw();
		} else
		{
			nextUI->draw();

		}
		retryUI->draw();
		selectFrame->draw();
	} else if(failedFlag) {
		failedUI->draw();
		if (failedUI->GetUiVisible()) {
			retryUI->draw();
			selectFrame->draw();
			undoRetryUI->draw();
		}
		
	}
}

void GameManagement::selectFunc() {
	if(!clearFlag && !failedFlag) {
		return;
	}
	if(Input::IsTriggerKey(KeyID::A)) {
		selectIndex--;
		operation->restart();//選択時の音
	}
	if(Input::IsTriggerKey(KeyID::D)) {
		selectIndex++;
		operation->restart();//選択時の音
	}
	selectIndex = std::clamp(selectIndex,0,1);

	if(failedFlag) {
		//selectIndex = 0;
		retryUI->get_transform().set_translate({458,169});

	} else if(clearFlag) {


		retryUI->get_transform().set_translate({458,247});

	}

	if(selectIndex == 0) {
		retryUI->get_uv_transform().set_translate_x(0);
		nextUI->get_uv_transform().set_translate_x(0.5f);
		goSelect->get_uv_transform().set_translate_x(0.5f);
		undoRetryUI->get_uv_transform().set_translate_x(0.5f);


		selectFrame->get_transform().set_translate(retryUI->get_transform().get_translate());
	} else {
		retryUI->get_uv_transform().set_translate_x(0.5f);
		nextUI->get_uv_transform().set_translate_x(0);
		goSelect->get_uv_transform().set_translate_x(0);
		undoRetryUI->get_uv_transform().set_translate_x(0);

		//選択中のUIの位置にフレームを移動
		if(clearFlag){
			selectFrame->get_transform().set_translate(nextUI->get_transform().get_translate());

		} else if(failedFlag){
			selectFrame->get_transform().set_translate(undoRetryUI->get_transform().get_translate());

		}

	}

}
