#pragma once

#include <Library/Math/Transform2D.h>

#include <memory>

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

public:
	void SetReasonIndex(int value) { reasonIndex = value; }
	bool GetUiVisible() { return uiVisibleFlag; }
	bool GetCanOperation() { return canOperation; }

private:
	void EaseChange(int index, float easeT);
	void updateReason();
private:
	std::unique_ptr<SpriteInstance>alignmentTex;
	std::unique_ptr<SpriteInstance>letterTex[7];//1文字づつ表示用

	Vector2 endPos[7];//最終的な位置
	Vector2 startPos[7];//最終的な位置

	float curEaseT[7];
	float totalEaseT = 0.8f;
	float endPosHeight = 415.f;//最終的な位置の高さ

	int curIndex;
	float delayTotalTime = 0.1f;
	float curDelayTime;


	std::unique_ptr<SpriteInstance>failedReasonUI;
	int reasonIndex;
	float reaCurEaseT;
	float reaTotalEaseT = 0.8f;
	Vector2 newScale;
	bool reaUpdateFlag;

	bool uiVisibleFlag;//リザルトの操作UIを表示するか
	bool canOperation;
};

