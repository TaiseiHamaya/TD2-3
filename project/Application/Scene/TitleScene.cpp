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


#include "Application/GameValue.h"
#include "Application/Scene/GameScene.h"


TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::load() {

	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/start.png");

}

void TitleScene::initialize() {
	Camera2D::Initialize();
	startUi = eps::CreateUnique<SpriteInstance>("start.png", Vector2{ 0.5f, 0.5f });
	startUi->get_transform().set_translate({ 640.0f,140 });

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

void TitleScene::popped() {}

void TitleScene::finalize() {}

void TitleScene::begin() {}

void TitleScene::update() {}

void TitleScene::begin_rendering() {
	startUi->begin_rendering();
}

void TitleScene::late_update() {}

void TitleScene::draw() const {
	renderPath->begin();
	// Mesh

	renderPath->next();
	// Sprite
	startUi->draw();

	renderPath->next();
}

#ifdef _DEBUG
void TitleScene::debug_update() {}
#endif // _DEBUG

