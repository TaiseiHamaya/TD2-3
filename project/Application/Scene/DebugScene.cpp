#include "DebugScene.h"

#ifdef _DEBUG

#include <Engine/Module/Render/RenderNode/Forward/Object3DNode/Object3DNode.h>
#include <Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Engine/Utility/Tools/SmartPointer.h>

#include "Application/Scene/GameScene.h"
#include "Application/Scene/SelectScene.h"

void DebugScene::initialize() {
	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({ object3dNode });
}

void DebugScene::update() {
	if (Input::IsPressKey(KeyID::F1)) {
		SceneManager::SetSceneChange(std::make_unique<SelectScene>(), 0);
	}
	else if (Input::IsPressKey(KeyID::F2)) {
		SceneManager::SetSceneChange(std::make_unique<GameScene>(level), 0);
	}
}

void DebugScene::draw() const {
	renderPath->begin();
	renderPath->next();
}

void DebugScene::debug_update() {
	ImGui::Begin("Debug");
	if (ImGui::Button("Select(Key : 1)")) {
		SceneManager::SetSceneChange(std::make_unique<SelectScene>(), 0);
	}
	if (ImGui::Button("Game(Key : 2)")) {
		SceneManager::SetSceneChange(std::make_unique<GameScene>(level), 0);
	}
	ImGui::InputScalar("Level", ImGuiDataType_U32, &level);
	ImGui::End();
}

#endif // _DEBUG
