#include "GameScene.h"
#include "Engine/Module/World/Camera/Camera3D.h"
#include "Engine/Resources/PolygonMesh/PolygonMeshManager.h"
#include "Engine/Module/Render/RenderPath/RenderPath.h"
#include <Engine/Utility/Tools/SmartPointer.h>
#include <Engine/Module/Render/RenderNode/Object3DNode/Object3DNode.h>
#include "Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h"
#include <Engine/Module/World/Camera/Camera2D.h>
#include <Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Module/Render/RenderNode/Sprite/SpriteNode.h>

GameScene::GameScene() = default;

GameScene::~GameScene() = default;

void GameScene::load()
{
	PolygonMeshManager::RegisterLoadQue("./EngineResources/Models", "Sphere.obj");
	PolygonMeshManager::RegisterLoadQue("./Resources/Models/cube", "cube.obj");
}

void GameScene::initialize()
{
	Camera2D::Initialize();
	camera3D = std::make_unique<Camera3D>();
	camera3D->initialize();
	camera3D->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(0,0,0),
		{0,2,-20}
		});

	packet = std::make_unique<Packet>();
	packet->initialize();
	packet->set_camera(camera3D.get());

	directionalLight = eps::CreateUnique<DirectionalLightInstance>();
	directionalLight->initialize();

	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(eps::to_bitflag(RenderNodeConfig::ContinueDrawAfter) | RenderNodeConfig::ContinueDrawBefore);
	spriteNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({ object3dNode, spriteNode });
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

	packet->update();
}

void GameScene::begin_rendering()
{
	camera3D->update_matrix();
	directionalLight->begin_rendering();

	packet->begin_rendering();
}

void GameScene::late_update()
{
}

void GameScene::draw() const
{
	renderPath->begin();
	directionalLight->register_world(3);
	camera3D->register_world(1);
	packet->draw();

	renderPath->next();

	packet->draw_sprite();
}

void GameScene::debug_update()
{
	packet->debug_update();
}
