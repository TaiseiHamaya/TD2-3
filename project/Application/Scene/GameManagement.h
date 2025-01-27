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

private:
	std::unique_ptr<SpriteInstance> clearSprite;
	std::unique_ptr<SpriteInstance> failedSprite;

	std::unique_ptr<SpriteInstance>nextUI;
	std::unique_ptr<SpriteInstance>retryUI;
	std::unique_ptr<SpriteInstance>selectFrame;

	bool clearFlag;
	bool failedFlag;

	bool isReset;

	int selectIndex; //0がリトライ、1がネクスト
};