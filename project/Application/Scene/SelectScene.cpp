#include "SelectScene.h"

#include "Engine/Module/Render/RenderNode/2D/Sprite/SpriteNode.h"
#include <Engine/Module/Render/RenderNode/Forward/Object3DNode/Object3DNode.h>
#include <Engine/Module/World/Camera/Camera2D.h>
#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Resources/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Resources/Texture/TextureManager.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Engine/Utility/Tools/SmartPointer.h>
#include "Engine/Resources/Audio/AudioManager.h"


#include "Application/GameValue.h"
#include "Application/Scene/GameScene.h"

SelectScene::SelectScene() : SelectScene(1) {};

SelectScene::SelectScene(int32_t selectLevel) :
	selectIndex(selectLevel) {
}

SelectScene::~SelectScene() = default;

void SelectScene::load() {
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/StageSelectUI.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/start.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/number.png");
	AudioManager::RegisterLoadQue("./GameResources/Audio/BGM/Title.wav");

}

void SelectScene::initialize() {
	// Instance
	Camera2D::Initialize();
	startUi = eps::CreateUnique<SpriteInstance>("start.png", Vector2{ 0.5f, 0.5f });
	startUi->get_transform().set_translate({ 640.0f,140 });
	selectUi = eps::CreateUnique<SpriteInstance>("StageSelectUI.png", Vector2{ 0.5f, 0.5f });
	selectUi->get_transform().set_translate({ 640.0f,550.0f });
	numberUi = eps::CreateUnique<SpriteInstance>("number.png", Vector2{ 0.5f, 0.5f });
	numberUi->get_transform().set_translate({ 640.0f,360.0f });
	numberUi->get_transform().set_scale({ 0.1f,1.0f });
	numberUi->get_uv_transform().set_scale({ 0.1f,1.0f });

	// Node&Path
	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueDrawBefore);
	spriteNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({ object3dNode,spriteNode });

	bgm = std::make_unique<AudioPlayer>();
	bgm->initialize("Game.wav");
	bgm->set_loop(true);
	bgm->set_volume(0.1f);
	bgm->play();
}

void SelectScene::popped() {
}

void SelectScene::finalize() {
}

void SelectScene::begin() {
}

void SelectScene::update() {
	if (Input::IsTriggerKey(KeyID::D) && selectIndex < GameValue::MaxLevel) {
		++selectIndex;
	}
	else if (Input::IsTriggerKey(KeyID::A) && selectIndex > 1) {
		--selectIndex;
	}

	numberUi->get_uv_transform().set_translate_x(selectIndex * 0.1f);

	if (Input::IsTriggerKey(KeyID::Space)) {
		SceneManager::SetSceneChange(
			eps::CreateUnique<GameScene>(selectIndex), 1.0f
		);
	}
}

void SelectScene::begin_rendering() {
	numberUi->begin_rendering();
	selectUi->begin_rendering();
	startUi->begin_rendering();
}

void SelectScene::late_update() {
}

void SelectScene::draw() const {
	renderPath->begin();
	// Mesh

	renderPath->next();
	// Sprite
	numberUi->draw();
	selectUi->draw();
	startUi->draw();

	renderPath->next();
}

#ifdef _DEBUG
void SelectScene::debug_update() {
	ImGui::Begin("SelectUi");
	selectUi->debug_gui();
	ImGui::End();
	ImGui::Begin("numberUi");
	numberUi->debug_gui();
	ImGui::End();
	ImGui::Begin("startUi");
	startUi->debug_gui();
	ImGui::End();

}
#endif // _DEBUG
