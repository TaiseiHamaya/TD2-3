#pragma once

#include <memory>

#include <Library/Utility/Template/Reference.h>

#include "Engine/Assets/Audio/AudioPlayer.h"

#include"Application/UI/FailedUI.h"
#include"Application/UI/ClearUI.h"

class SpriteInstance;
class SpriteDrawExecutor;

class GameManagement {
public:
	GameManagement();
	~GameManagement();

	void init();

	void begin();
	void update();
	void debug_update();

	void write_to_executor(Reference<SpriteDrawExecutor> executor0, Reference<SpriteDrawExecutor> executor1, Reference<SpriteDrawExecutor> executor2, Reference<SpriteDrawExecutor> executor3) const;

	void resultKeyInput();

	void on_reset();

private:
	void selectFunc();
	// リセットの更新処理
	void reset_update();
	// エスケープの処理
	void to_select_update();
public:
	//アクセッサ
	void SetClearFlag(bool value) { clearFlag = value; }
	bool GetClearFlag() { return clearFlag; }
	bool GetFailedFlag() { return failedFlag; }
	void SetFailedFlag(bool value) { failedFlag = value; }
	bool is_reset() const { return isReset; }

	bool is_restart() const { return isRestart; }
	bool is_next() const { return isNext; }
	bool is_undoRestart()const { return isUndoRestart; }
	bool is_escape_game() const { return toSelectTimer >= 1.0f; }
	void SetFailedSelect(int value) { failedUI->SetReasonIndex(value); }
	void SetCurLevel(int value) { curLevel = value; }
	void SetMaxLevel(int value) { maxLevel = value; }
	bool is_transition() const { return isTransition; }
	bool GetCanOperation() { return canOperation; }
	void SetIsTutorial(bool flag) { isTutorial = flag; }

	enum class ResetState {
		Idle,       // ボタンを押していない
		Pressing,   // 押し続けている最中
		Resetting,  // リセット中（ちょうどリセットが発動した瞬間）
		Completed   // リセット完了後
	};
	ResetState resetState;
	ResetState get_reset_state() const { return resetState; }
private:
	//std::unique_ptr<SpriteInstance> clearSprite;
	//std::unique_ptr<SpriteInstance> failedSprite;

	std::unique_ptr<SpriteInstance>nextUI;
	std::unique_ptr<SpriteInstance>retryUI;
	std::unique_ptr<SpriteInstance>undoRetryUI;
	std::unique_ptr<SpriteInstance>goSelect;
	std::unique_ptr<SpriteInstance>selectFrame;
	//std::unique_ptr<SpriteInstance>failedReasonUI;//0:子どもを置いてゴール 1:子コアラがゴール　2:コアラを落とす　3:ターン経過
	std::unique_ptr<SpriteInstance>resetBack;
	std::unique_ptr<SpriteInstance>resetKoara;
	std::unique_ptr<SpriteInstance>toSelectBack;
	std::unique_ptr<SpriteInstance>toSelectKoara;

	std::unique_ptr<FailedUI>failedUI;
	std::unique_ptr<ClearUI>clearUI;

	bool isTransition{ false };

	bool clearFlag;
	bool failedFlag;
	bool isRestart;

	float selectInputTimer;
	float SelectInputTime;




	const float resetMaxTime = 0.5f;
	float resetCurrentTime;
	bool isReset;
	bool isTutorial;

	ResetState toSelectState;
	const float toSelectMaxTime = 0.5f;
	float toSelectCurrentTime;
	//bool isToSelect;
	//bool isTutorial;

	bool isNext;
	bool isUndoRestart;

	int selectIndex; //0がリトライ、1がネクスト
	//int failedSelectIndex;//0:子どもを置いてゴール 1:子コアラがゴール　2:コアラを落とす　3:ターン経過(まだない)

	float toSelectTimer;
	int curLevel;
	int maxLevel;

	//音関連
	std::unique_ptr<AudioPlayer> decision;//確定音
	std::unique_ptr<AudioPlayer> operation;//切り替え音
	std::unique_ptr<AudioPlayer> clearAudio;//切り替え音
	std::unique_ptr<AudioPlayer> failedAudio;//切り替え音
	bool resultSoundFlag;

	bool canOperation;

	std::unique_ptr< AudioPlayer>resetAudio;
	std::unique_ptr< AudioPlayer>backTitle;
	std::unique_ptr<AudioPlayer> undoAudio;//undoの音


};