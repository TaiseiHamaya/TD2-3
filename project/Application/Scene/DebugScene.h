#pragma once

#ifdef _DEBUG

#include "Engine/Runtime/Scene/BaseScene.h"

#include <memory>

#include <Engine/Module/Render/RenderPath/RenderPath.h>

class DebugScene : public BaseScene {
public:
	DebugScene() = default;
	~DebugScene() = default;

public:
	void initialize() override;
	void update() override;
	void draw() const override;
public:
	void debug_update() override;

private:
	std::unique_ptr<RenderPath> renderPath;
	uint32_t level;
};

#endif // _DEBUG
