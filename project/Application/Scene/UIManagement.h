#pragma once
#include <memory>
#include "Engine/Debug/ImGui/ImGuiLoadManager/ImGuiLoadManager.h"


class SpriteInstance;

class UIManagement{
public:
	UIManagement();
	~UIManagement();

	void init();
	void begin();
	void update();
	void begin_rendering();
	void darw();

public:
	//アクセッサ
	void SetClearFlag(bool value) { clearFlag = value; }
	void SetFailedFlag(bool value) { failedFlag = value; }
	bool is_reset() const { return isReset; }

private:
	std::unique_ptr<SpriteInstance> clearSprite;
	std::unique_ptr<SpriteInstance> failedSprite;

	std::unique_ptr<SpriteInstance>wasdSprite[4];

	bool clearFlag;
	bool failedFlag;

	bool isReset;
};