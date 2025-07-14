#pragma once

#include <memory>

#include <Engine/Module/World/Sprite/SpriteInstance.h>

enum class KeyID;

class GameSceneUI {
public:
	GameSceneUI();
	~GameSceneUI();

	void initialize(int32_t level);
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
	void padControl(int index);
	float OutBack(float t, float totaltime, float min, float max, float s);

private:

	static const  int uiIndex = 8;

	std::array<std::array<std::unique_ptr<SpriteInstance>, uiIndex>, 2> controlSprite;
	std::unique_ptr<SpriteInstance> noneButtonSprite;
	//std::unique_ptr<SpriteInstance>wasdSprite[];
	std::unique_ptr<SpriteInstance>tutorialUI;

	Vector2 numCenter;
	std::unique_ptr<SpriteInstance> stageFrame;
	std::unique_ptr<SpriteInstance> numberUi;
	std::unique_ptr<SpriteInstance> numberUi10;

	bool isCanRelese;
	bool popUpUI;

	float curEaseT;
	float totalEaseT = 0.3f;
	float ratio;

	int curLevel;

};

