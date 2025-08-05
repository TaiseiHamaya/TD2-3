#pragma once

#include <memory>

#include <Library/Utility/Template/Reference.h>

#include <Engine/Module/World/Sprite/SpriteInstance.h>

#include <Application/GameManagement/GameManagement.h>

class SpriteDrawExecutor;

class TutorialManager
{
public:
	TutorialManager();

public:
	void initialize(uint32_t stage);
	void update();

	void write_to_executor(Reference<SpriteDrawExecutor> executor) const;

#ifdef _DEBUG
	void debug_update();
#endif
	
	enum class TutorialState {
		Appearing,  // 出現中
		Staying,    // 表示中
		Disappearing // 消滅中
	};
	TutorialState state_ = TutorialState::Appearing;

	enum class TutorialStep {
		RescueChild,             // 子供を救出しよう
		MoveToRocket,            // ロケットに向かおう
		LetParentGoalFirst,      // 親を先にゴールさせよう
		HoldingHasDirection,     // 持ち方に向きがある
		DropOffChild,            // 子供を下す
		ParentChildSupportEach,  // 親と子は支えあう
		ChildSupportsParent,     // 子供が親を支えることもできる
		CannotProceedIfFall,     // 親子どちらも落ちてしまう場合は進めない
		IceSlippery,             // 氷の床は滑る
		NoSlideIfOneOnGround,    // 親子どちらかが床にいると滑らない
		StopWhenStuckTogether,   // くっついたら止まる
		Count,                   // 要素数の管理用
	};

	TutorialStep get_tutorial_step() const { return tutorialStep_; }

	// アクセッサ
	bool get_is_tutorial() const { return isTutorial_; }
	void set_is_tutorial(bool flag) { isTutorial_ = flag; }
	void set_game_management(GameManagement* gameManagement) { gameManager_ = gameManagement; }
private:

	// チュートリアル用スプライト
	std::unique_ptr<SpriteInstance> tutorialFrame_;
	std::unique_ptr<SpriteInstance> tutorialText_;
	std::unique_ptr<SpriteInstance> tutorialImage_;
	std::unique_ptr<SpriteInstance> Abutton_;

	// ステージ管理クラスのポインタ
	GameManagement* gameManager_;

	// 現段階のチュートリアルの段階
	TutorialStep tutorialStep_ = TutorialStep::RescueChild;
	// チュートリアルが表示されているかどうかのフラグ
	bool isTutorial_ = false;

	// タイマー軍
	float currentTimer_ = 0.0f;
	float maxTimer_ = 0.3f;
	float t_ = 0.0f;
};

