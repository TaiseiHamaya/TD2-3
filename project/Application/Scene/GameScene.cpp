#include "GameScene.h"
#include "Engine/Utility/Tools/SmartPointer.h"
#include "Engine/Module/Render/RenderPath/RenderPath.h"
#include "Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h"
#include "Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h"
#include "Engine/Module/Render/RenderNode/Object3DNode/Object3DNode.h"

GameScene::GameScene() = default;

GameScene::~GameScene() = default;

void GameScene::load()
{
}

void GameScene::initialize()
{


	player = std::make_unique<Player>();
	player->initialize();

	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
	//object3dNode->set_render_target(renderTarget);
	//object3dNode->set_config(RenderNodeConfig::ContinueDrawBefore | RenderNodeConfig::ContinueUseDpehtBefore);

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({object3dNode});
}

void GameScene::popped()
{
}

void GameScene::finalize()
{
}

void GameScene::begin()
{
}

void GameScene::update()
{
}

void GameScene::begin_rendering()
{
	player->begin_rendering();
}

void GameScene::late_update()
{
}

void GameScene::draw() const
{
	renderPath->begin();
	player->draw();
}

#ifdef _DEBUG
void GameScene::debug_update()
{
}
#endif // _DEBUG

