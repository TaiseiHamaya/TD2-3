#pragma once
#include <memory>
#include "Engine/Resources/Audio/AudioPlayer.h"
#include <Engine/Module/World/Particle/ParticleEmitterInstance.h>


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
private:
	std::unique_ptr<AnimatedMeshInstance> animatedMeshInstance;
	bool isResult;
	// パーティクルの召還
	std::unique_ptr<ParticleEmitterInstance> gushingEmitter;

};

