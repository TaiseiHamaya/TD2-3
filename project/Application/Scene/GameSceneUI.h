#pragma once

#include <memory>
#include "Engine/Debug/ImGui/ImGuiLoadManager/ImGuiLoadManager.h"
#include "Library/Math/Transform2D.h"
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

public:
	//アクセッサ
	void setIsCanRelese(bool value) { isCanRelese = value; }
private:
	void keyControl(int index);

private:

	static const  int uiIndex = 7;

	std::array<std::unique_ptr<SpriteInstance>, uiIndex>wasdSprite;
	//std::unique_ptr<SpriteInstance>wasdSprite[];
	std::unique_ptr<SpriteInstance>tutorialUI;

	bool isCanRelese;


};

