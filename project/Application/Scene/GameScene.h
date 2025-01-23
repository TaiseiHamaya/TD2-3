#pragma once
#include "Engine/Runtime/Scene/BaseScene.h"
#include <memory>
#include "Engine/Module/World/Light/DirectionalLight/DirectionalLightInstance.h"

class RenderPath;
class Camera3D;
#include "Application/Character/Player/PlayerManager.h"
#include "Application/ClearUIClass.h"



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
	std::unique_ptr<PlayerManager> playerManager;
	std::unique_ptr<ClearUIClass> clearUI;

	std::unique_ptr<Camera3D> camera3D;
	std::unique_ptr<DirectionalLightInstance> directionalLight;
};

