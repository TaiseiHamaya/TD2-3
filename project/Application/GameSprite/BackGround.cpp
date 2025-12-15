#include "BackGround.h"

#include <Library/Math/Transform2D.h>

#include <Engine/Module/DrawExecutor/2D/SpriteDrawExecutor.h>
#include <Engine/Module/DrawExecutor/Mesh/SkinningMeshDrawManager.h>
#include <Engine/Module/World/Mesh/SkinningMeshInstance.h>
#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Runtime/Clock/WorldClock.h>

BackGround::BackGround() {
	backGroundSprite = std::make_unique<SpriteInstance>("backGround2.png");
	backGroundSprite2 = std::make_unique<SpriteInstance>("backGround.png");
	animatedMeshInstance = std::make_unique<SkinningMeshInstance>("GoalObj.gltf", "Standby", false);
	auto& objMat = animatedMeshInstance->get_materials();
	for (auto& mat : objMat) {
		mat.lightingType = LighingType::None;
	}
	animatedMeshInstance->get_transform().set_quaternion(
	Quaternion::EulerDegree(30.f, 0.f, -40.f));

	gushingEmitter = std::make_unique<ParticleEmitterInstance>("gushing2.json", 128);
	coolTime = 0;
	easeT = totalEaseT;

	gushingEmitter->get_transform().set_translate_x(-100.f);
	gushingEmitter->update_affine();
	//gushingEmitter->set_active(false);
}

BackGround::~BackGround() = default;

void BackGround::setup(Reference<SkinningMeshDrawManager> manager) {
	manager->register_instance(animatedMeshInstance);
}

void BackGround::update() {
	timer += WorldClock::DeltaSeconds();
	curScroll = scrollSpeed * timer;
	curScroll2 = scrollSpeed2 * timer;
	backGroundSprite->get_material().uvTransform.set_translate_x(curScroll);
	backGroundSprite2->get_material().uvTransform.set_translate_x(curScroll2);

	animatedMeshInstance->begin();
	rocketUpdate();
	animatedMeshInstance->update();
	animatedMeshInstance->update_affine();
	animatedMeshInstance->update_animation();
	gushingEmitter->update();

	coolTime += WorldClock::DeltaSeconds();
	if (coolTime >= 60) {
		coolTime = 0;
		easeT = 0;
	}

	gushingEmitter->transfer();
}

#ifdef _DEBUG

#include <imgui.h>
void BackGround::debugUpdate() {
	//ImGui::Begin("BackGroundParticle");
	//gushingEmitter->debug_gui();
	//	ImGui::End();
}
#endif

void BackGround::write_to_executor(Reference<SpriteDrawExecutor> executor) const {
	executor->write_to_buffer(backGroundSprite);
	executor->write_to_buffer(backGroundSprite2);
}

void BackGround::drawParticle() {
	gushingEmitter->draw();
}

void BackGround::rocketUpdate() {
	easeT += WorldClock::DeltaSeconds();

	float ratio = std::clamp(easeT / totalEaseT, 0.f, 1.f);

	Vector3 newPos = Vector3::Lerp(rocektStartPos, rocketEndPos, ratio);
	animatedMeshInstance->get_transform().set_translate(newPos);
	gushingEmitter->get_transform().set_translate(newPos);

	animatedMeshInstance->update_affine();
	gushingEmitter->update_affine();
}
