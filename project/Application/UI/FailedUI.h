#pragma once

#include <memory>
#include "Engine/Debug/ImGui/ImGuiLoadManager/ImGuiLoadManager.h"
#include "Library/Math/Transform2D.h"
#include "Engine/Runtime/WorldClock/WorldClock.h"
class SpriteInstance;

class FailedUI
{
public:
	FailedUI();
	~FailedUI();

	void init();
	void update();
	void debugUpdate();
	void begin_rendering();
	void draw();

private:
	std::unique_ptr<SpriteInstance>failedTex;
	std::unique_ptr<SpriteInstance>failedLetter[7];//1文字づつ表示用

	Vector2 endPos[7];//最終的な位置
	float endPosHeight = 415.f;//最終的な位置の高さ

};

