#include "Rocket.h"
#include "Engine/Module/World/AnimatedMesh/AnimatedMeshInstance.h"
#include<Engine/Resources/Animation/NodeAnimation/NodeAnimationPlayer.h>

Rocket::Rocket(const Vector3& pos) {
	animatedMeshInstance = std::make_unique<AnimatedMeshInstance>("GoalObj.gltf", "Standby", false);

	Vector3 newPos = pos;
	newPos.y = 0.5f;
	animatedMeshInstance->get_transform().set_translate(newPos);
	init();

}

Rocket::~Rocket() {}

void Rocket::init() {
	animatedMeshInstance->get_animation()->reset_animation("Standby");
	animatedMeshInstance->get_animation()->restart();
	isResult = false;

	gushingEmitter = std::make_unique<ParticleEmitterInstance>("gushing.json", 128);
	explosionEmitter = std::make_unique<ParticleEmitterInstance>("explosion.json", 128);


}

void Rocket::update(int state) {

	gushingEmitter->update();
	explosionEmitter->update();
	animatedMeshInstance->begin();
	//クリア時
	if (state == 1 || state == 3) {
		isClear();
	}
	else if (state == 2) {
		isFailed();
	}

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
}

void Rocket::isFailed() {
	if (isResult)return;
	isResult = true;
	animatedMeshInstance->get_animation()->reset_animation("Failed");
	animatedMeshInstance->get_animation()->restart();

}
