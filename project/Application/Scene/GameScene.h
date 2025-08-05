#pragma once
#include "Engine/Runtime/Scene/BaseScene.h"

#include <memory>
#include <vector>

class Camera3D;
class LevelLoader;

#include "Application/Character/Player/PlayerManager.h"
#include "Application/Mapchip/MapchipField.h"
#include "Application/GameManagement/GameManagement.h"
#include "Application/GameSprite/GameSceneUI.h"
#include "Application/GameSprite/BackGround.h"

#include <Engine/Module/Render/RenderPath/RenderPath.h>
#include <Engine/GraphicsAPI/DirectX/DxResource/TextureResource/RenderTexture.h>
#include <Engine/Module/World/Light/DirectionalLight/DirectionalLightInstance.h>
#include <Engine/Module/Render/RenderNode/Posteffect/Outline/OutlineNode.h>
#include <Engine/Module/DrawExecutor/Mesh/SkinningMeshDrawManager.h>
#include <Engine/Module/DrawExecutor/Mesh/StaticMeshDrawManager.h>
#include <Engine/Module/DrawExecutor/2D/SpriteDrawExecutor.h>
#include <Engine/Module/DrawExecutor/LightingExecutor/DirectionalLightingExecutor.h>
#include <Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h>

#include "Application/Rocket/Rocket.h"
#include "Application/Tutorial/TutorialManager.h"
#include "Application/PostEffect/GaussianBlurNode.h"

#include "Application/PostEffect/GaussianBlurNode.h"

class LuminanceExtractionNode;
class MargeTextureNode;
class BloomNode;
class GaussianBlurNode;

class LuminanceExtractionNode;
class MargeTextureNode;
class BloomNode;
class GaussianBlurNode;

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
	std::vector<RenderTexture> renderTextures;
	SingleRenderTarget meshRT;
	SingleRenderTarget baseRenderTexture;
	SingleRenderTarget luminanceRenderTexture;
	SingleRenderTarget downSampleRenderTexture2;
	SingleRenderTarget downSampleRenderTexture4;
	SingleRenderTarget downSampleRenderTexture8;
	SingleRenderTarget downSampleRenderTexture16;
	SingleRenderTarget bloomBaseRenderTexture;

	std::shared_ptr<OutlineNode> outlineNode;
	std::shared_ptr<LuminanceExtractionNode> luminanceExtractionNode;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode2;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode4;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode8;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode16;
	std::shared_ptr<MargeTextureNode> margeTextureNode;
	std::shared_ptr<BloomNode> bloomNode;

	std::unique_ptr<SkinningMeshDrawManager> skinningMeshDrawManager;
	std::unique_ptr<StaticMeshDrawManager> staticMeshDrawManager;
	std::unique_ptr<SpriteDrawExecutor> bgSpriteDrawExecutor;
	std::vector<std::unique_ptr<SpriteDrawExecutor>> spriteDrawExecutors;
	std::unique_ptr<DirectionalLightingExecutor> directionalLightingExecutor;

	// プレイヤーの生成
	std::unique_ptr<PlayerManager> playerManager;
	std::unique_ptr<MapchipField> fieldObjs;
	std::unique_ptr<GameManagement> managementUI;//ゲームクリア、ゲームオーバーをまとめて管理してる
	// チュートリアルの管理
	std::unique_ptr<TutorialManager> tutorialManager;

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

#ifdef _DEBUG
	GaussianBlurNode::GaussianBlurInfo blurData{
		.dispersion = 1.0f,
		.length = 40.0f,
		.sampleCount = 8
	};
#endif

};

