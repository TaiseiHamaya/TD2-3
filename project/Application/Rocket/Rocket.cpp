#include "Rocket.h"
#include "Engine/Module/World/AnimatedMesh/AnimatedMeshInstance.h"

Rocket::Rocket() { init(); }

Rocket::~Rocket() {}

void Rocket::init() {
	animatedMeshInstance = std::make_unique<AnimatedMeshInstance>("Goal.gltf", "Standby", true);

}

void Rocket::update() {
	animatedMeshInstance->begin();


	animatedMeshInstance->update();

}

void Rocket::begin() {
	

}

void Rocket::begin_rendering() {
	animatedMeshInstance->begin_rendering();

}
#ifdef _DEBUG

#include <imgui.h>
void Rocket::debug_update() {
	ImGui::Begin("AnimatedMesh");
	animatedMeshInstance->debug_gui();
		ImGui::End();
}
#endif // _DEBUG

void Rocket::draw() {
	animatedMeshInstance->draw();
}
