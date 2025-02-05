#include "TitleScene.h"
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
#include <Engine/Module/Render/RenderNode/Forward/SkinningMesh/SkinningMeshNode.h>

#include "Application/GameValue.h"
#include "Application/Scene/SelectScene.h"

#include <utility>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::load() {

	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/start.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/black.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/TitleLogo.png");
	AudioManager::RegisterLoadQue("./GameResources/Audio/BGM/TitleBGM.wav");

}

void TitleScene::initialize() {
	Camera2D::Initialize();
	startUi = eps::CreateUnique<SpriteInstance>("start.png", Vector2{ 0.5f, 0.5f });
	startUi->get_transform().set_translate({ 640.0f,140 });
	titleLogo = eps::CreateUnique<SpriteInstance>("TitleLogo.png", Vector2{ 0.5f, 0.5f });
	titleLogo->get_transform().set_translate({ 640.0f,440 });

	transition = eps::CreateUnique<SpriteInstance>("black.png", Vector2{ 0.f, 0.f });

	// Node&Path
	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_config(RenderNodeConfig::ContinueDrawBefore | RenderNodeConfig::ContinueUseDpehtBefore);
	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	std::shared_ptr<SkinningMeshNode> skinningMeshNode;
	skinningMeshNode = std::make_unique<SkinningMeshNode>();
	skinningMeshNode->initialize();
	skinningMeshNode->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueDrawBefore | RenderNodeConfig::ContinueUseDpehtAfter);
	skinningMeshNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(RenderNodeConfig::ContinueDrawAfter);
	spriteNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({ object3dNode,skinningMeshNode,spriteNode });

	bgm = std::make_unique<AudioPlayer>();
	bgm->initialize("TitleBGM.wav");
	bgm->set_loop(true);
	bgm->set_volume(0.2f);
	bgm->play();
}

void TitleScene::popped() {}

void TitleScene::finalize() {}

void TitleScene::begin() {}

void TitleScene::update() {
	switch (sceneState) {
	case TransitionState::In:
		in_update();
		break;
	case TransitionState::Main:
		default_update();
		break;
	case TransitionState::Out:
		out_update();
		break;
	}
}

void TitleScene::begin_rendering() {
	startUi->begin_rendering();
	titleLogo->begin_rendering();
	transition->begin_rendering();
}

void TitleScene::late_update() {}

void TitleScene::draw() const {
	renderPath->begin();
	// Mesh
	renderPath->next();

	// SkinningMesh
	renderPath->next();

	// Sprite
	startUi->draw();
	titleLogo->draw();
	transition->draw();

	renderPath->next();
}

void TitleScene::in_update() {
	transitionTimer += WorldClock::DeltaSeconds();
	float parametric = transitionTimer / 1.0f;
	transition->get_color().alpha = 1 - std::min(1.0f, parametric);
	if (parametric >= 1.0f) {
		sceneState = TransitionState::Main;
	}
}

void TitleScene::default_update() {
	if (Input::IsTriggerKey(KeyID::Space)) {
		SceneManager::SetSceneChange(
			eps::CreateUnique<SelectScene>(), 1.0f);
		transitionTimer = WorldClock::DeltaSeconds();
		sceneState = TransitionState::Out;
	}
}

void TitleScene::out_update() {
	transitionTimer += WorldClock::DeltaSeconds();
	float parametric = transitionTimer / 1.0f;
	transition->get_color().alpha = parametric;
}

#ifdef _DEBUG
void TitleScene::debug_update() {}
#endif // _DEBUG

