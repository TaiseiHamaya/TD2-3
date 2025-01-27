#pragma once

#include <memory>
#include "Engine/Debug/ImGui/ImGuiLoadManager/ImGuiLoadManager.h"

class SpriteInstance;


class GameSceneUI{
public:
	GameSceneUI();
	~GameSceneUI();

	void init();
	void update();
	void begin_rendering();
	void darw();

public:
	//アクセッサ

private:

	std::unique_ptr<SpriteInstance>wasdSprite[4];


};

