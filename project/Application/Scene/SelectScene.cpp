#include "SelectScene.h"

#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Engine/Utility/Tools/SmartPointer.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Module/Render/RenderNode/Forward/Object3DNode/Object3DNode.h>

#include "Application/Scene/GameScene.h"

void SelectScene::load() {
}

void SelectScene::initialize() {
	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({ object3dNode });
}

void SelectScene::popped() {
}

void SelectScene::finalize() {
}

void SelectScene::begin() {
}

void SelectScene::update() {
}

void SelectScene::begin_rendering() {
}

void SelectScene::late_update() {
}

void SelectScene::draw() const {
	renderPath->begin();
	renderPath->next();
}

void SelectScene::debug_update() {

}
