#include "SelectScene.h"

#include <Engine/Module/Render/RenderNode/Forward/Object3DNode/Object3DNode.h>
#include <Engine/Module/World/Camera/Camera2D.h>
#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Resources/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Resources/Texture/TextureManager.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Engine/Utility/Tools/SmartPointer.h>

#include "Application/Scene/GameScene.h"

SelectScene::SelectScene() : SelectScene(0) {};

SelectScene::SelectScene(uint32_t selectLevel) :
	selectIndex(selectLevel) {
}

SelectScene::~SelectScene() = default;

void SelectScene::load() {
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/StageSelectUI.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/number.png");
}

void SelectScene::initialize() {
	// Instance
	Camera2D::Initialize();
	selectUi = eps::CreateUnique<SpriteInstance>("StageSelectUI.png");
	numberUi = eps::CreateUnique<SpriteInstance>("number.png");
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
}

void SelectScene::popped() {
}

void SelectScene::finalize() {
}

void SelectScene::begin() {
}

void SelectScene::update() {
	if (Input::IsTriggerKey(KeyID::D)) {
		++selectIndex;
	}
	else if (Input::IsTriggerKey(KeyID::A)) {
		--selectIndex;
	}

	numberUi->get_uv_transform().set_translate_x(selectIndex * 0.1f);
}

void SelectScene::begin_rendering() {
	numberUi->begin_rendering();
	selectUi->begin_rendering();
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

	renderPath->next();
}

void SelectScene::debug_update() {

}
