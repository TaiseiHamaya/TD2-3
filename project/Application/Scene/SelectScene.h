#pragma once

#include "Engine/Runtime/Scene/BaseScene.h"

#include <memory>

#include "Application/GameSprite/BackGround.h"
#include <Application/Mapchip/MapchipField.h>
#include <Engine/Module/Render/RenderPath/RenderPath.h>
#include <Engine/Module/World/Camera/Camera3D.h>
#include <Engine/Module/World/Light/DirectionalLight/DirectionalLightInstance.h>
#include <Engine/Resources/Audio/AudioPlayer.h>

#include "Application/PostEffect/GaussianBlurNode.h"
#include "Engine/Module/Render/RenderNode/Posteffect/Outline/OutlineNode.h"

class LuminanceExtractionNode;
class MargeTextureNode;
class BloomNode;
class GaussianBlurNode;

class MeshInstance;
class AnimatedMeshInstance;
class SpriteInstance;

class SelectScene : public BaseScene {
private:
	enum class TransitionState {
		In,
		Main,
		Out
	};

public:
	SelectScene();
	SelectScene(int32_t selectLevel, bool isFromGame);
	~SelectScene();

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

public:
	void crate_field_view();

private:
	void in_update();
	void default_update();
	void out_update();

	void from_game_update();

#ifdef _DEBUG
public:
	void debug_update() override;
#endif // _DEBUG

private:
	TransitionState sceneState;

	float transitionTimer{ 0 };
	float inputTimer{ 0 };
	float InputDowntime{ 0 };
	std::unique_ptr<SpriteInstance> transition;

	std::unique_ptr<SpriteInstance>fromGameBack;
	std::unique_ptr<SpriteInstance>fromGameKoara;

	std::unique_ptr<RenderPath> renderPath;

	std::shared_ptr<OutlineNode> outlineNode;

	std::shared_ptr<LuminanceExtractionNode> luminanceExtractionNode;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode2;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode4;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode8;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode16;
	std::shared_ptr<MargeTextureNode> margeTextureNode;
	std::shared_ptr<BloomNode> bloomNode;

	std::unique_ptr<Camera3D> camera3D;
	std::unique_ptr<DirectionalLightInstance> directionalLight;

	std::unique_ptr<AnimatedMeshInstance> parentKoala;
	std::unique_ptr<AnimatedMeshInstance> childKoala;
	std::unique_ptr<MeshInstance> goalMesh;

	int32_t selectIndex;
	std::unique_ptr<SpriteInstance> selectUi;
	std::array<std::unique_ptr<SpriteInstance>, 2> startUi;
	std::unique_ptr<SpriteInstance> numberUi;
	std::unique_ptr<SpriteInstance> numberUi10;
	std::unique_ptr<SpriteInstance> obSprite;
	std::array<std::unique_ptr<SpriteInstance>, 2> backTitleSprite;

	std::unique_ptr<AudioPlayer> bgm;

	std::unique_ptr<WorldInstance> fieldRotation;
	std::unique_ptr<MapchipField> field;

	Quaternion startRotation;

	float fadeEase;//フェードインアウトのイージング

	float test{ 0 };

	std::unique_ptr< BackGround>background;

	std::unique_ptr< AudioPlayer>startAudio;
	std::unique_ptr< AudioPlayer>selectAudio;
	std::unique_ptr< AudioPlayer>backTitle;

	const float fromGameMaxTime = 0.5f;
	float fromGameCurrentTime;
	bool isFromGameScene = false;

#ifdef _DEBUG
	GaussianBlurNode::GaussianBlurInfo blurData{
		.dispersion = 1.0f,
		.length = 40.0f,
		.sampleCount = 8
	};
#endif
};
