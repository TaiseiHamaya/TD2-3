#pragma once

#include "Engine/Runtime/Scene/BaseScene.h"

#include <memory>

#include <Engine/Module/Render/RenderPath/RenderPath.h>
#include "Engine/Resources/Audio/AudioPlayer.h"
#include <Library/Math/Vector3.h>

class Camera3D;
class DirectionalLightInstance;
class SpriteInstance;
class AnimatedMeshInstance;

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

	std::unique_ptr<Camera3D> camera3D;
	std::unique_ptr<DirectionalLightInstance> directionalLight;

	std::unique_ptr<SpriteInstance> transition;
	std::unique_ptr<SpriteInstance> startUi;
	std::unique_ptr<SpriteInstance> titleLogo;

	//BGM
	std::unique_ptr<AudioPlayer>bgm;

	std::unique_ptr<AnimatedMeshInstance> parentObj;
	std::unique_ptr<AnimatedMeshInstance> chiledObj;

	float easeT;
	float totalEaseT = 6.0f;
	Vector3 movePos;

	std::unique_ptr< AudioPlayer>startAudio;
};

