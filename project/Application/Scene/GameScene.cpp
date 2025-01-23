#include "GameScene.h"
#include "Engine/Utility/Tools/SmartPointer.h"
#include "Engine/Module/Render/RenderPath/RenderPath.h"
#include "Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h"
#include "Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h"
#include "Engine/Module/Render/RenderNode/Object3DNode/Object3DNode.h"
#include "Engine/Resources/PolygonMesh/PolygonMeshManager.h"
#include "Engine/Resources/Animation/NodeAnimation/NodeAnimationManager.h"
#include "Engine/Resources/Animation/Skeleton/SkeletonManager.h"
#include "Engine/Module/World/Camera/Camera2D.h"
#include "Engine/Module/World/Camera/Camera3D.h"
#include "Engine/Resources/Texture/TextureManager.h"


GameScene::GameScene() = default;

GameScene::~GameScene() = default;

void GameScene::load()
{
	//PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/Primitive/Sphere.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ParentObj/ParentObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ChildObj/ChildObj.obj");
	TextureManager::RegisterLoadQue("./GameResources/Texture/ClearTex.png");

}

void GameScene::initialize()
{
	Camera2D::Initialize();
	camera3D = std::make_unique<Camera3D>();
	camera3D->initialize();
	camera3D->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(45,0,0),
		{0,10,-10}
		});

	playerManager = std::make_unique<PlayerManager>();
	playerManager->initialize();

	directionalLight = eps::CreateUnique<DirectionalLightInstance>();

	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({object3dNode});

	clearUI = std::make_unique<ClearUIClass>();
	
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
	playerManager->update();
	directionalLight->update();
	clearUI->update();
}

void GameScene::begin_rendering()
{
	playerManager->begin_rendering();

	camera3D->update_matrix();
	directionalLight->begin_rendering();
	clearUI->begin_rendering();

}

void GameScene::late_update()
{
}

void GameScene::draw() const
{
	renderPath->begin();
	camera3D->register_world(1);
	directionalLight->register_world(3);

	playerManager->draw();

	renderPath->next();
	clearUI->darw();
}

#ifdef _DEBUG
void GameScene::debug_update()
{
}
#endif // _DEBUG

