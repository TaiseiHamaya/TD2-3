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
#include <Engine/Runtime/WorldClock/WorldClock.h>


GameScene::GameScene() = default;

GameScene::~GameScene() = default;

void GameScene::load()
{
	//PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/Primitive/Sphere.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ParentObj/ParentObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ChildObj/ChildObj.obj");
	TextureManager::RegisterLoadQue("./GameResources/Texture/ClearTex.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/FailedTex.png");

	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/RordObj/RordObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/WallObj/WallObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.obj");
}

void GameScene::initialize()
{
	Camera2D::Initialize();
	camera3D = std::make_unique<Camera3D>();
	camera3D->initialize();
	camera3D->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(30,0,0),
		{3,8,-10}
		});

	playerManager = std::make_unique<PlayerManager>();
	playerManager->initialize();

	fieldObjs = std::make_unique<MapchipField>();
	fieldObjs->init();

	playerManager->set_mapchip_field(fieldObjs.get());

	directionalLight = eps::CreateUnique<DirectionalLightInstance>();

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueDrawBefore);
	spriteNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({object3dNode,spriteNode });

	managementUI = std::make_unique<GameManagement>();

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
	WorldClock::Update();

	playerManager->update();
	fieldObjs->update();
	directionalLight->update();
	managementUI->update();
}

void GameScene::begin_rendering()
{
	playerManager->begin_rendering();
	fieldObjs->begin_rendering();

	camera3D->update_matrix();
	directionalLight->begin_rendering();
	managementUI->begin_rendering();

}

void GameScene::late_update()
{
}

void GameScene::draw() const
{
	renderPath->begin();
	camera3D->register_world(1);
	directionalLight->register_world(3);

	fieldObjs->draw();
	playerManager->draw();

	renderPath->next();
	managementUI->darw();
	renderPath->next();
}

#ifdef _DEBUG
void GameScene::debug_update()
{
	ImGui::Begin("Camera");
	camera3D->debug_gui();
	ImGui::End();

	playerManager->debug_update();
}
#endif // _DEBUG

