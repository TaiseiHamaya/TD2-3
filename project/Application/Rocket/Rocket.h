#pragma once
#include <memory>
#include "Engine/Resources/Audio/AudioPlayer.h"

class AnimatedMeshInstance;
#include "Engine/Module/Render/RenderNode/Forward/SkinningMesh/SkinningMeshNode.h"

class Rocket
{
public:
	Rocket();
	~Rocket();

	void init();
	void update() ;
	void begin();
	void begin_rendering() ;
	void debug_update() ;
	void draw();

private:
	std::unique_ptr<AnimatedMeshInstance> animatedMeshInstance;

};

