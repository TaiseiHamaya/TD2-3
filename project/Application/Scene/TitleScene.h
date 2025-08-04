#pragma once

#include "Engine/Runtime/Scene/BaseScene.h"

#include <array>
#include <memory>
#include <vector>

#include <Engine/Assets/Audio/AudioPlayer.h>
#include <Engine/GraphicsAPI/DirectX/DxResource/TextureResource/RenderTexture.h>
#include <Engine/Module/DrawExecutor/2D/SpriteDrawExecutor.h>
#include <Engine/Module/DrawExecutor/LightingExecutor/DirectionalLightingExecutor.h> 
#include <Engine/Module/DrawExecutor/Mesh/SkinningMeshDrawManager.h>
#include <Engine/Module/DrawExecutor/Mesh/StaticMeshDrawManager.h>
#include <Engine/Module/Render/RenderPath/RenderPath.h>
#include <Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h>

#include <Library/Math/Vector3.h>

class Camera3D;
class DirectionalLightInstance;
class SpriteInstance;
class SkinningMeshInstance;

#include "Application/Rocket/Rocket.h"
#include "Application/Tutorial/TutorialManager.h"
#include "Application/PostEffect/GaussianBlurNode.h"

class LuminanceExtractionNode;
class MargeTextureNode;
class BloomNode;
class GaussianBlurNode;

class TitleScene : public BaseScene {
private:
	enum class TransitionState {
		In,
		Main,
		Out
	};

public:
	TitleScene();
	~TitleScene();

	void load()override;
	void initialize() override;
	void popped() override;
	void finalize() override;

	void begin() override;
	void update() override;
	void begin_rendering() override;
	void late_update() override;

	void draw() const override;

private:
	void in_update();
	void default_update();
	void out_update();

#ifdef _DEBUG
public:
	void debug_update() override;
#endif // _DEBUG

private:
	TransitionState sceneState;
	float transitionTimer{ 0 };

	std::unique_ptr<RenderPath> renderPath;
	std::vector<RenderTexture> renderTextures;
	SingleRenderTarget baseRenderTexture;
	SingleRenderTarget luminanceRenderTexture;
	SingleRenderTarget downSampleRenderTexture2;
	SingleRenderTarget downSampleRenderTexture4;
	SingleRenderTarget downSampleRenderTexture8;
	SingleRenderTarget downSampleRenderTexture16;
	SingleRenderTarget bloomBaseRenderTexture;

	std::shared_ptr<LuminanceExtractionNode> luminanceExtractionNode;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode2;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode4;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode8;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode16;
	std::shared_ptr<MargeTextureNode> margeTextureNode;
	std::shared_ptr<BloomNode> bloomNode;

	std::unique_ptr<SkinningMeshDrawManager> skinningMeshDrawManager;
	std::unique_ptr<StaticMeshDrawManager> staticMeshDrawManager;
	std::unique_ptr<SpriteDrawExecutor> spriteDrawExecutor;
	std::unique_ptr<DirectionalLightingExecutor> directionalLightingExecutor;

	std::unique_ptr<Camera3D> camera3D;
	std::unique_ptr<DirectionalLightInstance> directionalLight;

	std::unique_ptr<SpriteInstance> transition;
	std::array<std::unique_ptr<SpriteInstance>, 4> startUi;
	std::unique_ptr<SpriteInstance> titleLogo;
	std::unique_ptr<SpriteInstance> languageSelection;

	//BGM
	std::unique_ptr<AudioPlayer>bgm;

	std::unique_ptr<SkinningMeshInstance> parentObj;
	std::unique_ptr<SkinningMeshInstance> chiledObj;

	float easeT;
	float totalEaseT = 6.0f;
	Vector3 movePos;

	std::unique_ptr< AudioPlayer>startAudio;

	float languageSelectTimer{ 0.0f };
	std::unique_ptr<AudioPlayer> selectSeSuccussed;
	std::unique_ptr<AudioPlayer> selectSeFailed;

#ifdef _DEBUG
	GaussianBlurNode::GaussianBlurInfo blurData{
		.dispersion = 1.0f,
		.length = 40.0f,
		.sampleCount = 8
	};
#endif
};
