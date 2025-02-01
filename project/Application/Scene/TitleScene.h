#pragma once

#include "Engine/Runtime/Scene/BaseScene.h"

#include <memory>

#include <Engine/Module/Render/RenderPath/RenderPath.h>
#include "Engine/Resources/Audio/AudioPlayer.h"

class SpriteInstance;

class TitleScene : public BaseScene
{
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

#ifdef _DEBUG
public:
	void debug_update() override;
#endif // _DEBUG

	std::unique_ptr<RenderPath> renderPath;

	std::unique_ptr<SpriteInstance> startUi;
	std::unique_ptr<SpriteInstance> titleLogo;

};

