#include "TutorialManager.h"
#include <Engine/Utility/Tools/SmartPointer.h>
#include <Library/Math/Transform2D.h>
#include <Engine/Runtime/Input/Input.h>

#ifdef _DEBUG
#include <imgui.h>
#endif
#include <Engine/Runtime/WorldClock/WorldClock.h>
#include <algorithm>

void TutorialManager::initialize(uint32_t stage) {
	tutorialFrame_ = eps::CreateUnique<SpriteInstance>("Frame.png");
	tutorialFrame_->set_active(false);
	tutorialText_ = eps::CreateUnique<SpriteInstance>("TutorialText.png");
	tutorialText_->get_transform().set_translate({ 80.0f, 160.0f });
	tutorialText_->get_transform().set_scale({ 1.0f, 0.1f });
	tutorialText_->get_uv_transform().set_scale({ 1.0f, 0.09f });
	tutorialText_->set_active(false);
	tutorialImage_ = eps::CreateUnique<SpriteInstance>("TutorialImage.png");
	tutorialImage_->get_transform().set_translate({ 160.0f, 320.0f });
	tutorialImage_->get_transform().set_scale({ 1.0f, 0.1f });
	tutorialImage_->get_uv_transform().set_scale({ 1.0f, 0.095f });
	tutorialImage_->set_active(false);

	if (stage == 1) {
		tutorialStep_ = TutorialStep::RescueChild;
	}
	else if (stage == 2) {
		tutorialStep_ = TutorialStep::HoldingHasDirection;
	}
	else if (stage == 5) {
		tutorialStep_ = TutorialStep::CannotProceedIfFall;
	}
	else if (stage == 11) {
		tutorialStep_ = TutorialStep::IceSlippery;
	}
	else {
		tutorialStep_ = TutorialStep::Count;
	}

	// 最初のテキストを用意しておく
	tutorialText_->get_uv_transform().set_translate_y(0.0875f * static_cast<int>(tutorialStep_));
	tutorialImage_->get_uv_transform().set_translate_y(0.091f * static_cast<int>(tutorialStep_));
}

void TutorialManager::update() {
	// チュートリアルが起動されてたらそれ用の処理にする
	if (isTutorial_) {

		switch (state_) {
		case TutorialState::Appearing:
			// 見えるようにしとく
			tutorialFrame_->set_active(true);
			tutorialText_->set_active(true);
			tutorialImage_->set_active(true);

			// 今の時間にΔタイムを足す
			currentTimer_ += WorldClock::DeltaSeconds();
			// Clamp 0.0f ～ 1.0f
			t_ = std::clamp(currentTimer_ / maxTimer_, 0.0f, 1.0f);
			tutorialText_->get_color().alpha = t_;
			tutorialFrame_->get_color().alpha = t_;
			tutorialImage_->get_color().alpha = t_;

			if (currentTimer_ >= maxTimer_) {
				state_ = TutorialState::Staying;
				currentTimer_ = 0.0f;
			}

			break;
		case TutorialState::Staying:

			// もしキー入力されたら
			if (Input::IsTriggerKey(KeyID::Space)) {
				state_ = TutorialState::Disappearing;
			}
			break;
		case TutorialState::Disappearing:
			currentTimer_ += WorldClock::DeltaSeconds();

			// Clamp 0.0f ～ 1.0f
			t_ = std::clamp(1.0f - (currentTimer_ / maxTimer_), 0.0f, 1.0f);
			tutorialText_->get_color().alpha = t_;
			tutorialFrame_->get_color().alpha = t_;
			tutorialImage_->get_color().alpha = t_;

			if (currentTimer_ >= maxTimer_) {
				// 進める処理
				if (tutorialStep_ != TutorialStep::Count &&
					tutorialStep_ != TutorialStep::LetParentGoalFirst &&
					tutorialStep_ != TutorialStep::ParentChildSupportEach &&
					tutorialStep_ != TutorialStep::ChildSupportsParent) {
					tutorialStep_ = static_cast<TutorialStep>(static_cast<int>(tutorialStep_) + 1);
				}
				else {
					tutorialStep_ = TutorialStep::Count;
				}

				// 見えなくする
				tutorialFrame_->set_active(false);
				tutorialText_->set_active(false);
				tutorialImage_->set_active(false);
				// 次のテキストを用意しておく
				tutorialText_->get_uv_transform().set_translate_y(0.0875f * static_cast<int>(tutorialStep_));
				tutorialImage_->get_uv_transform().set_translate_y(0.091f * static_cast<int>(tutorialStep_));

				// チュートリアルの終了
				isTutorial_ = false;

				state_ = TutorialState::Appearing;
				currentTimer_ = 0.0f;
			}
			break;
		}
	}
}

void TutorialManager::begin_rendering() {
	tutorialFrame_->begin_rendering();
	tutorialText_->begin_rendering();
	tutorialImage_->begin_rendering();
}

void TutorialManager::draw() {
	tutorialFrame_->draw();
	tutorialText_->draw();
	tutorialImage_->draw();
}

#ifdef _DEBUG
void TutorialManager::debug_update() {
	ImGui::Begin("TutorialText");
	tutorialText_->debug_gui();
	ImGui::End();

	ImGui::Begin("TutorialText");
	tutorialImage_->debug_gui();
	ImGui::End();
}
#endif // _DEBUG

