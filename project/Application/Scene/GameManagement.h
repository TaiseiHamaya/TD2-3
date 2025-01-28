#pragma once
#include <memory>
#include "Engine/Debug/ImGui/ImGuiLoadManager/ImGuiLoadManager.h"
#include "Library/Math/Transform2D.h"


class SpriteInstance;

class GameManagement{
public:
	GameManagement();
	~GameManagement();

	void init();
	void begin();
	void update();
	void debug_update();
	void begin_rendering();
	void darw();

private:
	void selectFunc();
public:
	//アクセッサ
	void SetClearFlag(bool value) { clearFlag = value; }
	void SetFailedFlag(bool value) { failedFlag = value; }
	bool is_reset() const { return isReset; }
	bool is_next() const { return isNext; }
	void SetFailedSelect(int value){ failedSelectIndex = value; }

private:
	std::unique_ptr<SpriteInstance> clearSprite;
	std::unique_ptr<SpriteInstance> failedSprite;

	std::unique_ptr<SpriteInstance>nextUI;
	std::unique_ptr<SpriteInstance>retryUI;
	std::unique_ptr<SpriteInstance>selectFrame;
	std::unique_ptr<SpriteInstance>failedUI;//0:子どもを置いてゴール 1:子コアラがゴール　2:コアラを落とす　3:ターン経過

	bool clearFlag;
	bool failedFlag;

	bool isReset;
	bool isNext;

	int selectIndex; //0がリトライ、1がネクスト
	int failedSelectIndex;//0:子どもを置いてゴール 1:子コアラがゴール　2:コアラを落とす　3:ターン経過(まだない)

};