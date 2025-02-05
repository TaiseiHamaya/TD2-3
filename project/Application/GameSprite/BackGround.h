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
	void drawParticle();
	void rocketUpdate();
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

	Vector3 rocektStartPos = {-11.4f,-22.f,11.8f};
	Vector3 rocketEndPos = {11.4f,7.6f,11.8f};
	float easeT;
	float totalEaseT = 10.f;
	std::unique_ptr<ParticleEmitterInstance> gushingEmitter;

	float coolTime;
};

