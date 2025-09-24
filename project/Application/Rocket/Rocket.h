#pragma once

#include <memory>

#include <Engine/Assets/Audio/AudioPlayer.h>
#include <Engine/Module/World/Particle/ParticleEmitterInstance.h>

#include <Library/Math/Vector3.h>
#include <Library/Utility/Template/Reference.h>

class SkinningMeshInstance;
class SkinningMeshDrawManager;

class Rocket
{
public:
	Rocket(const Vector3& position);
	~Rocket();

	void init();

	void setup(Reference<SkinningMeshDrawManager> drawManger);

	void update(int state);
	void begin();
	void update_affine();
	//void debug_update();

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

