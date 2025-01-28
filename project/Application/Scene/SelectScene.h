#pragma once

#include "Engine/Runtime/Scene/BaseScene.h"

#include <memory>

#include <Engine/Module/Render/RenderPath/RenderPath.h>

class SpriteInstance;

class SelectScene : public BaseScene {
public:
	SelectScene();
	SelectScene(int32_t selectLevel);
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

#ifdef _DEBUG
public:
	void debug_update() override;
#endif // _DEBUG

private:
	std::unique_ptr<RenderPath> renderPath;

	int32_t selectIndex;
	std::unique_ptr<SpriteInstance> selectUi;
	std::unique_ptr<SpriteInstance> startUi;
	std::unique_ptr<SpriteInstance> numberUi;
};
