#pragma once
#include "Engine/Runtime/Scene/BaseScene.h"
#include <memory>

class RenderPath;

#include "Application/Character/Player/Player.h"

class GameScene : public BaseScene
{
public:
	GameScene();
	~GameScene();

public:
	void load() override;
	void initialize() override;
	void popped() override;
	void finalize() override;

	void begin() override;
	void update() override;
	void begin_rendering() override;
	void late_update() override;

	void draw() const override;

	//void on_collision(const BaseCollider* const, Color4* object);
	//void on_collision_enter(const BaseCollider* const, Color4* object);
	//void on_collision_exit(const BaseCollider* const, Color4* object);

#ifdef _DEBUG
	void debug_update() override;
#endif // _DEBUG

private:

	std::unique_ptr<RenderPath> renderPath;
	// プレイヤーの生成
	std::unique_ptr<Player> player;
};

