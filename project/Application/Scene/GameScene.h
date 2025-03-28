#pragma once
#include "Engine/Runtime/Scene/BaseScene.h"

#include <memory>

#include "Engine/Module/World/Light/DirectionalLight/DirectionalLightInstance.h"

class RenderPath;
class Camera3D;
class LevelLoader;

#include "Application/Character/Player/PlayerManager.h"
#include "Application/Mapchip/MapchipField.h"
#include "Application/GameManagement/GameManagement.h"
#include "Application/GameSprite/GameSceneUI.h"
#include "Application/GameSprite/BackGround.h"

#include "Engine/Module/Render/RenderNode/Posteffect/Outline/OutlineNode.h"

#include "Application/Rocket/Rocket.h"

class GameScene : public BaseScene {
private:
	enum class TransitionState {
		In,
		Main,
		Out
	};

public:
	GameScene();
	GameScene(int32_t level);
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

private:
	void reset_level();

#ifdef _DEBUG
public:
	void debug_update() override;
#endif // _DEBUG
private:
	std::unique_ptr<RenderPath> renderPath;

	std::shared_ptr<OutlineNode> outlineNode;
	// プレイヤーの生成
	std::unique_ptr<PlayerManager> playerManager;
	std::unique_ptr<MapchipField> fieldObjs;
	std::unique_ptr<GameManagement> managementUI;//ゲームクリア、ゲームオーバーをまとめて管理してる

	std::unique_ptr<Camera3D> camera3D;
	std::unique_ptr<DirectionalLightInstance> directionalLight;

	const int32_t currentLevel;
	std::unique_ptr<LevelLoader> levelLoader;

	// Transition関連
	TransitionState sceneState;
	float transitionTimer{ 0 };
	std::unique_ptr<SpriteInstance> transition;
	std::unique_ptr<Rocket> rocketObj;

	//UI
	std::unique_ptr<GameSceneUI> gameUI;

	std::unique_ptr<BackGround>background;

	//BGM
	std::unique_ptr<AudioPlayer>bgm;

	float sceneChangeTime = 0.5f;

};

