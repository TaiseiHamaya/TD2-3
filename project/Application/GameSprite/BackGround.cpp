#include "BackGround.h"

#include <Library/Math/Transform2D.h>
#include "Engine/Module/World/AnimatedMesh/AnimatedMeshInstance.h"

#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Runtime/WorldClock/WorldClock.h>
#include "Engine/Rendering/DirectX/DirectXResourceObject/ConstantBuffer/Material/Material.h"

BackGround::BackGround() {
	backGroundSprite = std::make_unique<SpriteInstance>("backGround2.png");
	backGroundSprite2 = std::make_unique<SpriteInstance>("backGround.png");
	animatedMeshInstance = std::make_unique<AnimatedMeshInstance>("GoalObj.gltf", "Standby", false);
	auto& objMat = animatedMeshInstance->get_materials();
	for (auto& mat : objMat) {
		mat.lightingType = LighingType::None;
	}
	animatedMeshInstance->get_transform().set_quaternion(
	Quaternion::EulerDegree(30.f, 0.f, -40.f));

	gushingEmitter = std::make_unique<ParticleEmitterInstance>("gushing2.json", 128);
	coolTime = 0;
	easeT = totalEaseT;
}

BackGround::~BackGround() {}

void BackGround::update() {
	timer += WorldClock::DeltaSeconds();
	curScroll = scrollSpeed * timer;
	curScroll2 = scrollSpeed2 * timer;
	backGroundSprite->get_uv_transform().set_translate_x(curScroll);
	backGroundSprite2->get_uv_transform().set_translate_x(curScroll2);

	animatedMeshInstance->begin();
	rocketUpdate();
	animatedMeshInstance->update();
	gushingEmitter->update();

	coolTime += WorldClock::DeltaSeconds();
	if (coolTime >= 60) {
		coolTime = 0;
		easeT = 0;
	}
}

#ifdef _DEBUG

#include <imgui.h>
void BackGround::debugUpdate() {
	ImGui::Begin("BackGroundParticle");
	gushingEmitter->debug_gui();
		ImGui::End();
}
#endif
void BackGround::begin_rendering() {

	backGroundSprite->begin_rendering();
	backGroundSprite2->begin_rendering();
	animatedMeshInstance->begin_rendering();
	gushingEmitter->begin_rendering();
}

void BackGround::draw() {
	backGroundSprite->draw();
	backGroundSprite2->draw();
}

void BackGround::animeDraw() {
	animatedMeshInstance->draw();
}

void BackGround::drawParticle() {
	gushingEmitter->draw();
}

void BackGround::rocketUpdate() {

	easeT += WorldClock::DeltaSeconds();

	float ratio = std::clamp(easeT/totalEaseT,0.f,1.f);

	Vector3 newPos = Vector3::Lerp(rocektStartPos, rocketEndPos, ratio);
	animatedMeshInstance->get_transform().set_translate(newPos);
	gushingEmitter->get_transform().set_translate(newPos);

}
