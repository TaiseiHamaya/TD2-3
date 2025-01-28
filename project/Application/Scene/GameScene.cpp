#include "GameScene.h"

#include "Engine/Module/Render/RenderNode/Forward/Object3DNode/Object3DNode.h"
#include "Engine/Module/Render/RenderPath/RenderPath.h"
#include "Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h"
#include "Engine/Module/World/Camera/Camera2D.h"
#include "Engine/Module/World/Camera/Camera3D.h"
#include "Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h"
#include "Engine/Resources/Animation/NodeAnimation/NodeAnimationManager.h"
#include "Engine/Resources/Animation/Skeleton/SkeletonManager.h"
#include "Engine/Resources/PolygonMesh/PolygonMeshManager.h"
#include "Engine/Resources/Texture/TextureManager.h"
#include "Engine/Utility/Tools/SmartPointer.h"
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Engine/Runtime/WorldClock/WorldClock.h>

#include "Application/GameValue.h"
#include "Application/Scene/SelectScene.h"

#include "Application/LevelLoader/LevelLoader.h"

#ifdef _DEBUG
#include "Engine/Module/Render/RenderNode/Debug/PrimitiveLine/PrimitiveLineNode.h"
#endif // _DEBUG

GameScene::GameScene() : GameScene(1) {
}

GameScene::GameScene(uint32_t level) {
	currentLevel = level;
}
;

GameScene::~GameScene() = default;

void GameScene::load() {
	//PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/Primitive/Sphere.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.obj");
	TextureManager::RegisterLoadQue("./GameResources/Texture/ClearTex.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/FailedTex.png");

	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/RordObj/RordObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/WallObj/WallObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/IceObj/IceObj.obj");

	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Wkey.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Akey.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Skey.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Dkey.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ResetUI.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ESCkey.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ReleseUI.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Tutorial1.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Next.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Retry.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/SelectFrame.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/FailedUI_1.png");

}

void GameScene::initialize() {
	Camera2D::Initialize();
	camera3D = std::make_unique<Camera3D>();
	camera3D->initialize();
	camera3D->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(40,0,0),
		{3,10,-9}
		});

	levelLoader = eps::CreateUnique<LevelLoader>(currentLevel);

	fieldObjs = std::make_unique<MapchipField>();
	fieldObjs->initialize(levelLoader);

	playerManager = std::make_unique<PlayerManager>();
	playerManager->initialize(levelLoader, fieldObjs.get());

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

#ifdef _DEBUG
	std::shared_ptr<PrimitiveLineNode> primitiveLineNode;
	primitiveLineNode = std::make_unique<PrimitiveLineNode>();
	primitiveLineNode->initialize();
#endif // _DEBUG


	renderPath = eps::CreateUnique<RenderPath>();
#ifdef _DEBUG
	renderPath->initialize({ object3dNode,spriteNode,primitiveLineNode });
#else
	renderPath->initialize({ object3dNode,spriteNode });
#endif // _DEBUG

	managementUI = std::make_unique<GameManagement>();
	playerManager->set_game_management(managementUI.get());

	gameUI = std::make_unique<GameSceneUI>();
}

void GameScene::popped() {
}

void GameScene::finalize() {
}

void GameScene::begin() {
	managementUI->begin();
	if (managementUI->is_reset()) {
		fieldObjs->initialize(levelLoader);
		playerManager->initialize(levelLoader, fieldObjs.get());
	}
	else if (managementUI->is_next()) {
		// 最後のレベルではない場合
		if (currentLevel < GameValue::MaxLevel) {
			SceneManager::SetSceneChange(
				eps::CreateUnique<GameScene>(currentLevel + 1), 1.0
			);
		}
		// 最後のレベルの場合
		else {
			// TODO : ここに最後のレベルの場合の処理を書く
		}
	}
	else if (managementUI->is_escape_game()) {
		SceneManager::SetSceneChange(
			eps::CreateUnique<SelectScene>(currentLevel), 1.0
		);
	}
}

void GameScene::update() {
	//WorldClock::Update();

	playerManager->update();
	fieldObjs->update();
	directionalLight->update();
	managementUI->update();
	gameUI->update();
}

void GameScene::begin_rendering() {
	playerManager->begin_rendering();
	fieldObjs->begin_rendering();

	camera3D->update_matrix();
	directionalLight->begin_rendering();
	managementUI->begin_rendering();
	gameUI->begin_rendering();
}

void GameScene::late_update() {
}

void GameScene::draw() const {
	renderPath->begin();
	camera3D->register_world_projection(1);
	camera3D->register_world_lighting(4);
	directionalLight->register_world(5);

	fieldObjs->draw();
	playerManager->draw();

#ifdef _DEBUG
	camera3D->debug_draw_axis();
#endif // _DEBUG

	renderPath->next();

	managementUI->darw();
	gameUI->darw();
	renderPath->next();

#ifdef _DEBUG
	camera3D->register_world_projection(1);
	camera3D->debug_draw_frustum();

	renderPath->next();
#endif // _DEBUG

}

#ifdef _DEBUG
void GameScene::debug_update() {
	ImGui::Begin("Camera");
	camera3D->debug_gui();
	ImGui::End();

	playerManager->debug_update();
	gameUI->debugUpdate();
	managementUI->debug_update();
	ImGui::Begin("WorldClock");
	WorldClock::DebugGui();
	ImGui::End();
}
#endif // _DEBUG
