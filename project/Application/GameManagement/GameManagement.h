#pragma once

#include <memory>
#include "Engine/Resources/Audio/AudioPlayer.h"
#include"Application/UI/FailedUI.h"
#include"Application/UI/ClearUI.h"

class SpriteInstance;

class GameManagement {
public:
	GameManagement();
	~GameManagement();

	void init();
	void begin();
	void update();
	void debug_update();
	void begin_rendering();
	void darw();
	void resultKeyInput();

private:
	void selectFunc();
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
	bool is_escape_game() const { return toSelectTimer >= 0.1f; }
	void SetFailedSelect(int value) { failedUI->SetReasonIndex(value); }
	void SetCurLevel(int value) { curLevel = value; }
	void SetMaxLevel(int value) { maxLevel = value; }
	bool is_transition() const { return isTransition; }
	bool GetCanOperation() { return canOperation; }
private:
	//std::unique_ptr<SpriteInstance> clearSprite;
	//std::unique_ptr<SpriteInstance> failedSprite;

	std::unique_ptr<SpriteInstance>nextUI;
	std::unique_ptr<SpriteInstance>retryUI;
	std::unique_ptr<SpriteInstance>undoRetryUI;
	std::unique_ptr<SpriteInstance>goSelect;
	std::unique_ptr<SpriteInstance>selectFrame;
	//std::unique_ptr<SpriteInstance>failedReasonUI;//0:子どもを置いてゴール 1:子コアラがゴール　2:コアラを落とす　3:ターン経過

	std::unique_ptr< FailedUI>failedUI;
	std::unique_ptr< ClearUI>clearUI;

	bool isTransition{ false };

	bool clearFlag;
	bool failedFlag;
	bool isRestart;

	bool isReset;
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