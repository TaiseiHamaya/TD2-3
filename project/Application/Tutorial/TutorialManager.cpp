#include "TutorialManager.h"

#include <algorithm>

#include <Library/Math/Transform2D.h>
#include <Library/Utility/Tools/SmartPointer.h>

#include <Engine/Module/DrawExecutor/2D/SpriteDrawExecutor.h>
#include <Engine/Runtime/Clock/WorldClock.h>
#include <Engine/Runtime/Input/Input.h>

#include "Application/Configuration/Configuration.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

TutorialManager::TutorialManager() {
	tutorialFrame_ = eps::CreateUnique<SpriteInstance>("Frame.png");
	switch (Configuration::GetLanguage()) {
	case Configuration::Language::Japanese:
		tutorialText_ = eps::CreateUnique<SpriteInstance>("TutorialText.png");
		break;
	case Configuration::Language::English:
		tutorialText_ = eps::CreateUnique<SpriteInstance>("TutorialText_EN.png");
		break;
	}
	tutorialImage_ = eps::CreateUnique<SpriteInstance>("TutorialImage.png");
	Abutton_ = eps::CreateUnique<SpriteInstance>("Abutton.png");
}

void TutorialManager::initialize(uint32_t stage) {
	tutorialFrame_->set_active(false);
	tutorialText_->get_transform().set_translate({ 80.0f, 120.0f });
	tutorialText_->get_transform().set_scale({ 1.0f, 0.1f });
	tutorialText_->get_material().uvTransform.set_scale({ 1.0f, 0.09f });
	tutorialText_->set_active(false);
	tutorialImage_->get_transform().set_translate({ -40.0f, 200.0f });
	tutorialImage_->get_transform().set_scale({ 1.3f, 0.13f });
	tutorialImage_->get_material().uvTransform.set_scale({ 1.0f, 0.095f });
	tutorialImage_->set_active(false);
	Abutton_->get_transform().set_translate({ 1080.0f, 60.0f });
	Abutton_->set_active(false);

	if (stage == 1) {
		tutorialStep_ = TutorialStep::RescueChild;
	}
	else if (stage == 2) {
		tutorialStep_ = TutorialStep::HoldingHasDirection;
	}
	else if (stage == 5) {
		tutorialStep_ = TutorialStep::ChildSupportsParent;
	}
	else if (stage == 9) {
		tutorialStep_ = TutorialStep::IceSlippery;
	}
	else {
		tutorialStep_ = TutorialStep::Count;
	}

	// 最初のテキストを用意しておく
	tutorialText_->get_material().uvTransform.set_translate_y(0.0875f * static_cast<int>(tutorialStep_));
	tutorialImage_->get_material().uvTransform.set_translate_y(0.091f * static_cast<int>(tutorialStep_));
}

void TutorialManager::update() {
	// ゲームの管理クラスに今チュートリアルが出てるかどうかを送っておく
	gameManager_->SetIsTutorial(isTutorial_);

	// チュートリアルが起動されてたらそれ用の処理にする
	if (isTutorial_) {

		switch (state_) {
		case TutorialState::Appearing:
			// 見えるようにしとく
			tutorialFrame_->set_active(true);
			tutorialText_->set_active(true);
			tutorialImage_->set_active(true);
			Abutton_->set_active(true);

			// 今の時間にΔタイムを足す
			currentTimer_ += WorldClock::DeltaSeconds();
			// Clamp 0.0f ～ 1.0f
			t_ = std::clamp(currentTimer_ / maxTimer_, 0.0f, 1.0f);
			tutorialText_->get_material().color.alpha = t_;
			tutorialFrame_->get_material().color.alpha = t_;
			tutorialImage_->get_material().color.alpha = t_;
			Abutton_->get_material().color.alpha = t_;

			if (currentTimer_ >= maxTimer_) {
				state_ = TutorialState::Staying;
				currentTimer_ = 0.0f;
			}

			break;
		case TutorialState::Staying:

			// もしキー入力されたら
			if (Input::IsTriggerKey(KeyID::Space) || Input::IsTriggerPad(PadID::A)) {
				state_ = TutorialState::Disappearing;
			}
			break;
		case TutorialState::Disappearing:
			currentTimer_ += WorldClock::DeltaSeconds();

			// Clamp 0.0f ～ 1.0f
			t_ = std::clamp(1.0f - (currentTimer_ / maxTimer_), 0.0f, 1.0f);
			tutorialText_->get_material().color.alpha = t_;
			tutorialFrame_->get_material().color.alpha = t_;
			tutorialImage_->get_material().color.alpha = t_;
			Abutton_->get_material().color.alpha = t_;

			if (currentTimer_ >= maxTimer_) {
				// 進める処理
				if (tutorialStep_ != TutorialStep::Count &&
					tutorialStep_ != TutorialStep::LetParentGoalFirst &&
					tutorialStep_ != TutorialStep::ParentChildSupportEach &&
					tutorialStep_ != TutorialStep::CannotProceedIfFall) {
					tutorialStep_ = static_cast<TutorialStep>(static_cast<int>(tutorialStep_) + 1);
				}
				else {
					tutorialStep_ = TutorialStep::Count;
				}

				// 見えなくする
				tutorialFrame_->set_active(false);
				tutorialText_->set_active(false);
				tutorialImage_->set_active(false);
				Abutton_->set_active(false);
				// 次のテキストを用意しておく
				tutorialText_->get_material().uvTransform.set_translate_y(0.0875f * static_cast<int>(tutorialStep_));
				tutorialImage_->get_material().uvTransform.set_translate_y(0.091f * static_cast<int>(tutorialStep_));

				// チュートリアルの終了
				isTutorial_ = false;

				state_ = TutorialState::Appearing;
				currentTimer_ = 0.0f;
			}
			break;
		}
	}
}

void TutorialManager::write_to_executor(Reference<SpriteDrawExecutor> executor0, Reference<SpriteDrawExecutor> executor1, Reference<SpriteDrawExecutor> executor2) const {
	executor1->write_to_buffer(tutorialFrame_);
	executor0->write_to_buffer(tutorialText_);
	executor2->write_to_buffer(tutorialImage_);
	executor0->write_to_buffer(Abutton_);
}

#ifdef _DEBUG
void TutorialManager::debug_update() {
	ImGui::Begin("TutorialText");
	Abutton_->debug_gui();
	ImGui::End();
}
#endif // _DEBUG

