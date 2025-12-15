#include "DebugScene.h"

#ifdef _DEBUG

#include <Engine/Module/Render/RenderNode/Forward/Mesh/StaticMeshNodeForward.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Library/Utility/Tools/SmartPointer.h>

#include "Application/Scene/GameScene.h"
#include "Application/Scene/SelectScene.h"

void DebugScene::initialize() {
	std::shared_ptr<StaticMeshNodeForward> object3dNode;
	object3dNode = std::make_unique<StaticMeshNodeForward>();
	object3dNode->initialize();
	object3dNode->set_render_target_SC();

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
	ImGui::InputInt("Level", &level);
	ImGui::End();
}

#endif // _DEBUG
