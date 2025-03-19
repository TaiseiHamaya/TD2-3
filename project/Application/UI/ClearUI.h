#pragma once

#include <memory>
#include "Engine/Debug/ImGui/ImGuiLoadManager/ImGuiLoadManager.h"
#include "Library/Math/Transform2D.h"
#include "Engine/Runtime/WorldClock/WorldClock.h"
#include "Library/Math/Easing.h"
#include <Engine/Runtime/Input/Input.h>

class SpriteInstance;

class ClearUI
{
public:
	ClearUI();
	~ClearUI();
	void init();
	void update();
	void debugUpdate();
	void begin_rendering();
	void draw();

public:
	//アクセッサ
	bool GetUiVisible() { return uiVisibleFlag; }
	bool GetCanOperation() { return canOperation; }

private:
	void EaseChange(int index, float easeT);
	void BoundEase(int index);
private:
	std::unique_ptr<SpriteInstance>alignmentTex;
	std::unique_ptr<SpriteInstance>letterTex[6];//1文字づつ表示用

	Vector2 endPos[6];//最終的な位置
	Vector2 startPos[6];//最終的な位置
	float curEaseT[6];
	float totalEaseT = 0.8f;
	float endPosHeight = 417.f;//最終的な位置の高さ
	
	float boundTotalEaseT = 0.5f;
	float boundCurEaseT[6];
	float boundEaseDir[6];
	float boundLength = 50.f;

	int curIndex;
	float delayTotalTime = 0.1f;
	float curDelayTime;

	bool uiVisibleFlag;//リザルトの操作UIを表示するか
	bool canOperation;//操作出来るか
};

