#pragma once

#include <memory>
#include "Engine/Debug/ImGui/ImGuiLoadManager/ImGuiLoadManager.h"
#include "Library/Math/Transform2D.h"
#include "Engine/Runtime/WorldClock/WorldClock.h"


class SpriteInstance;

class BackGround
{
public:
	BackGround();
	~BackGround();

	void init();
	void update();
	void debugUpdate();
	void begin_rendering();
	void darw();

private:
	std::unique_ptr<SpriteInstance>backGroundSprite;
	std::unique_ptr<SpriteInstance>backGroundSprite2;
	float scrollSpeed = 0.01f;
	float scrollSpeed2 = 0.015f;
	float curScroll;
	float curScroll2;

};

