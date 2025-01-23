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

GameScene::GameScene() = default;

GameScene::~GameScene() = default;

void GameScene::load()
{
	PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/Primitive/Sphere.obj");
	//PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/Primitive/Sphere");
	//PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/Player.gltf");
	//NodeAnimationManager::RegisterLoadQue("./EngineResources/Models/Player.gltf");
	//SkeletonManager::RegisterLoadQue("./EngineResources/Models/Player.gltf");
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

	player = std::make_unique<Player>();
	player->initialize();

	directionalLight = eps::CreateUnique<DirectionalLightInstance>();

	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
	object3dNode->set_config(RenderNodeConfig::ContinueDrawBefore | RenderNodeConfig::ContinueUseDpehtBefore);

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

	directionalLight->update();
}

void GameScene::begin_rendering()
{
	camera3D->update_matrix();
	directionalLight->begin_rendering();
	player->begin_rendering();
}

void GameScene::late_update()
{
}

void GameScene::draw() const
{
	renderPath->begin();
	camera3D->register_world(1);
	directionalLight->register_world(3);
	player->draw();

	//renderPath->next();
}

#ifdef _DEBUG
void GameScene::debug_update()
{
}
#endif // _DEBUG

