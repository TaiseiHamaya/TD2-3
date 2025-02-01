#pragma once

#include <memory>
#include "Engine/Debug/ImGui/ImGuiLoadManager/ImGuiLoadManager.h"
#include "Library/Math/Transform2D.h"
#include "Engine/Runtime/WorldClock/WorldClock.h"

enum class KeyID;
class SpriteInstance;


class GameSceneUI{
public:
	GameSceneUI();
	~GameSceneUI();

	void init();
	void update();
	void debugUpdate();
	void begin_rendering();
	void darw();
	void ReleseUIUpdate();

public:
	//アクセッサ
	void setIsCanRelese(bool value) { isCanRelese = value; }
	void SetCurLevel(int value) { curLevel = value; }
private:
	void keyControl(int index);
	float OutBack(float t, float totaltime, float min, float max, float s);

private:

	static const  int uiIndex = 8;

	std::array<std::unique_ptr<SpriteInstance>, uiIndex>wasdSprite;
	//std::unique_ptr<SpriteInstance>wasdSprite[];
	std::unique_ptr<SpriteInstance>tutorialUI;

	bool isCanRelese;

	float curEaseT;
	float totalEaseT = 0.3f;
	float ratio;

	int curLevel;

};

