#pragma once

#include <memory>
#include <Engine/Module/World/Particle/ParticleEmitterInstance.h>
#include <Library/Math/Vector3.h>

class SpriteInstance;
class AnimatedMeshInstance;

class BackGround
{
public:
	BackGround();
	~BackGround();

	void update();
	void debugUpdate();
	void begin_rendering();
	void draw();
	void animeDraw();

public:
	float get_duration() const { return timer; }

private:
	float timer{ 0 };
	std::unique_ptr<SpriteInstance> backGroundSprite;
	std::unique_ptr<SpriteInstance> backGroundSprite2;
	float scrollSpeed = 0.007f;
	float scrollSpeed2 = 0.01f;
	float curScroll{ 0 };
	float curScroll2{ 0 };
	std::unique_ptr<AnimatedMeshInstance> animatedMeshInstance;

};

