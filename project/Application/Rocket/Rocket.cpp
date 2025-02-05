#include "Rocket.h"
#include "Engine/Module/World/AnimatedMesh/AnimatedMeshInstance.h"
#include<Engine/Resources/Animation/NodeAnimation/NodeAnimationPlayer.h>

Rocket::Rocket(const Vector3& pos) {
	

	animatedMeshInstance = std::make_unique<AnimatedMeshInstance>("GoalObj.gltf", "Standby", false);
	gushingEmitter = std::make_unique<ParticleEmitterInstance>("gushing.json", 128);
	explosionEmitter = std::make_unique<ParticleEmitterInstance>("explosion.json", 128);
	explosion = std::make_unique <AudioPlayer>();
	explosion->initialize("explosion.wav");
	gushing = std::make_unique <AudioPlayer>();
	gushing->initialize("gushing.wav");
	init();
	Vector3 newPos = pos;
	newPos.y = 0.5f;
	animatedMeshInstance->get_transform().set_translate(newPos);
	particlePos = newPos;
	explosionEmitter->get_transform().set_translate(newPos);
}

Rocket::~Rocket() {}

void Rocket::init() {
	animatedMeshInstance->get_animation()->reset_animation("Standby");
	animatedMeshInstance->get_animation()->restart();
	isResult = false;
	isFailedFlag = false;

	gushingEmitter->set_active(false);
	explosionEmitter->set_active(false);
	spwanParticle = false;
	spwanParticle2 = false;
	particleSpawnTime = 0;
	explosionSpawnTime = 0;

	


}

void Rocket::update(int state) {
	animatedMeshInstance->begin();
	gushingEmitter->update();
	explosionEmitter->update();
	
	//クリア時
	if (state == 1 || state == 3) {
		isClear();
	}
	else if (state == 2) {
		isFailed();
	}

	

	particleUpdate();
	animatedMeshInstance->update();
}

void Rocket::begin() {


}

void Rocket::begin_rendering() {
	animatedMeshInstance->begin_rendering();
	gushingEmitter->begin_rendering();
	explosionEmitter->begin_rendering();

}
#ifdef _DEBUG

#include <imgui.h>
void Rocket::debug_update() {
	ImGui::Begin("gushing");
	gushingEmitter->debug_gui();
	ImGui::End();
	ImGui::Begin("Rocket");
	animatedMeshInstance->debug_gui();
	ImGui::End();
	ImGui::Begin("explosion");
	explosionEmitter->debug_gui();
	ImGui::End();
}
#endif // _DEBUG

void Rocket::draw() {
	animatedMeshInstance->draw();

}

void Rocket::draw_particle() {
	gushingEmitter->draw(); 
	explosionEmitter->draw();
}

void Rocket::isClear() {
	if (isResult)return;
	isResult = true;
	animatedMeshInstance->get_animation()->reset_animation("Clear");
	animatedMeshInstance->get_animation()->restart();
	particleSpawnTime = 0;
}

void Rocket::isFailed() {
	if (isResult)return;
	isResult = true;
	isFailedFlag = true;
	animatedMeshInstance->get_animation()->reset_animation("Failed");
	animatedMeshInstance->get_animation()->restart();

}

void Rocket::particleUpdate() {
	if (!isResult) { return; }

	particleSpawnTime += WorldClock::DeltaSeconds();

	if (particleSpawnTime >= 0.7f) {
		//発射のパーティクルを再生
		if (!spwanParticle) {
			spwanParticle = true;
			gushingEmitter->set_active(true);
			gushingEmitter->restart();
			gushing->restart();
		}
		

	}
	particlePos.y = animatedMeshInstance->get_animation()->calculate_translate("Rocket").y;
	gushingEmitter->get_transform().set_translate(particlePos);
	if (isFailedFlag) {
		explosionSpawnTime+= WorldClock::DeltaSeconds();

		if (explosionSpawnTime > 2.8f) {
			if (!spwanParticle2) {
				explosionEmitter->set_active(true);
				explosionEmitter->restart();
				spwanParticle2 = true;
				particlePos.y = 0.5f;
				explosionEmitter->get_transform().set_translate(particlePos);
				explosion->restart();
			}
		}
	}

}
