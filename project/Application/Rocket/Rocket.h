#pragma once
#include <memory>
#include "Engine/Assets/Audio/AudioPlayer.h"
#include <Engine/Module/World/Particle/ParticleEmitterInstance.h>
#include "Engine/Runtime/Clock/WorldClock.h"


class SkinningMeshInstance;
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
	std::unique_ptr<SkinningMeshInstance> animatedMeshInstance;
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

	std::unique_ptr<AudioPlayer>explosion;
	std::unique_ptr<AudioPlayer>gushing;
};

