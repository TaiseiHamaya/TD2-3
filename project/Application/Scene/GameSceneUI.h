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
	void begin_rendering();
	void darw();

public:
	//アクセッサ

private:
	void keyControl( int index);

private:

	std::unique_ptr<SpriteInstance>wasdSprite[4];


};

