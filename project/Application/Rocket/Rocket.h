#pragma once
#include <memory>
#include "Engine/Resources/Audio/AudioPlayer.h"
#include <Engine/Module/World/Particle/ParticleEmitterInstance.h>
#include "Engine/Runtime/WorldClock/WorldClock.h"


class AnimatedMeshInstance;
#include <Library/Math/Vector3.h>
class Rocket
{
public:
	Rocket(const Vector3& position);
	~Rocket();

	void init();
	void update(int state) ;
	void begin();
	void begin_rendering() ;
	void debug_update() ;
	void draw();
	void draw_particle();

	void isClear();
	void isFailed();
	void particleUpdate();
private:
	std::unique_ptr<AnimatedMeshInstance> animatedMeshInstance;
	bool isResult;
	// パーティクルの召還
	std::unique_ptr<ParticleEmitterInstance> gushingEmitter;
	std::unique_ptr<ParticleEmitterInstance> explosionEmitter;

	bool isFailedFlag;
	float particleSpawnTime;
	
	float explosionSpawnTime;
	bool spwanParticle;
	bool spwanParticle2;
	Vector3 particlePos;
};

