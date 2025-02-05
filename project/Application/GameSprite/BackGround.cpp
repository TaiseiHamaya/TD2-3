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
}

BackGround::~BackGround() {}

void BackGround::update() {
	timer += WorldClock::DeltaSeconds();
	curScroll = scrollSpeed * timer;
	curScroll2 = scrollSpeed2 * timer;
	backGroundSprite->get_uv_transform().set_translate_x(curScroll);
	backGroundSprite2->get_uv_transform().set_translate_x(curScroll2);

	animatedMeshInstance->begin();
	animatedMeshInstance->update();
}

#ifdef _DEBUG

#include <imgui.h>
void BackGround::debugUpdate() {
	ImGui::Begin("Rocket");
	animatedMeshInstance->debug_gui();
		ImGui::End();
}
#endif
void BackGround::begin_rendering() {

	backGroundSprite->begin_rendering();
	backGroundSprite2->begin_rendering();
	animatedMeshInstance->begin_rendering();
}

void BackGround::draw() {
	backGroundSprite->draw();
	backGroundSprite2->draw();
}

void BackGround::animeDraw() {
	animatedMeshInstance->draw();
}
