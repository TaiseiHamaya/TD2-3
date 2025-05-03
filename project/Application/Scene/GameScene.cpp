#include "GameScene.h"

#include <Engine/Module/Render/RenderNode/2D/Sprite/SpriteNode.h>
#include <Engine/Module/Render/RenderNode/Forward/Object3DNode/Object3DNode.h>
#include <Engine/Module/Render/RenderNode/Forward/Particle/ParticleMeshNode/ParticleMeshNode.h>
#include <Engine/Module/Render/RenderNode/Forward/SkinningMesh/SkinningMeshNode.h>
#include <Engine/Module/Render/RenderPath/RenderPath.h>
#include <Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h>
#include <Engine/Module/World/Camera/Camera2D.h>
#include <Engine/Module/World/Camera/Camera3D.h>
#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Rendering/DirectX/DirectXResourceObject/DepthStencil/DepthStencil.h>
#include <Engine/Rendering/DirectX/DirectXResourceObject/OffscreenRender/OffscreenRender.h>
#include <Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Resources/Animation/NodeAnimation/NodeAnimationManager.h>
#include <Engine/Resources/Animation/Skeleton/SkeletonManager.h>
#include <Engine/Resources/Audio/AudioManager.h>
#include <Engine/Resources/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Resources/Texture/TextureManager.h>
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Engine/Runtime/WorldClock/WorldClock.h>
#include <Engine/Utility/Tools/SmartPointer.h>

#include "Application/GameValue.h"
#include "Application/Scene/SelectScene.h"
#include "Application/Scene/TitleScene.h"

#include "Application/LevelLoader/LevelLoader.h"

#ifdef _DEBUG
#include "Engine/Module/Render/RenderNode/Debug/PrimitiveLine/PrimitiveLineNode.h"
#endif // _DEBUG

GameScene::GameScene() : GameScene(1) {}

GameScene::GameScene(int32_t level) :
	currentLevel(level) {}
;

GameScene::~GameScene() = default;

void GameScene::load() {
	//PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/Primitive/Sphere.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/CatchEffect/CatchEffect.gltf");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ReleaseEffect/ReleaseEffect.gltf");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/exclamation/exclamation.gltf");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/FlusteredEffect/FlusteredEffect.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/CatchEffect/CatchEffect.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/ReleaseEffect/ReleaseEffect.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/exclamation/exclamation.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/FlusteredEffect/FlusteredEffect.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/CatchEffect/CatchEffect.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/ReleaseEffect/ReleaseEffect.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/exclamation/exclamation.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/FlusteredEffect/FlusteredEffect.gltf");
	TextureManager::RegisterLoadQue("./GameResources/Texture/ClearTex.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/FailedTex.png");

	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");


	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Wkey.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Akey.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Skey.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Dkey.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ResetUI.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ESCkey.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ReleseUI.png");

	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ResetUIController.png"); 
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ESCkeyController.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/UndoController.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ReleseUIController.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/NoneButton.png");
	
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Tutorial1.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Next.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Retry.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/SelectFrame.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/FailedUI_1.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/undoRetry.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/GoSelect.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Undo.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/smallNumber.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/stageFrame.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/backGround.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/backGround2.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Failed/Failed.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Failed/F.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Failed/a.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Failed/i.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Failed/l.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Failed/e.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Failed/d.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Failed/ten.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Clear/Clear.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Clear/C.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Clear/L.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Clear/E.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Clear/A.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Clear/R.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Clear/!.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Tutorial/Frame.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Tutorial/TutorialText.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/Tutorial/TutorialImage.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/KoaraFace.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ResetBack.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/Abutton.png");

	AudioManager::RegisterLoadQue("./GameResources/Audio/move.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/hold.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/release.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/fall.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/unmovable.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/iceMove.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/decision.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/operation.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/clearSound.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/failedSound.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/rotate.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/undo.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/explosion.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/gushing.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/reset.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/backAudio.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/BGM/Game.wav");

}

void GameScene::initialize() {
	Camera2D::Initialize();
	camera3D = std::make_unique<Camera3D>();
	camera3D->initialize();

	levelLoader = eps::CreateUnique<LevelLoader>(currentLevel);

	fieldObjs = std::make_unique<MapchipField>();
	fieldObjs->initialize(levelLoader);

	camera3D->set_transform({
		CVector3::BASIS,
		 Quaternion::EulerDegree(40,0,0),//Quaternion::AngleAxis(CVector3::BASIS_Y, -PI /4) *
		{ static_cast<float>(fieldObjs->column() - 1) / 2,10,-12 + static_cast<float>(fieldObjs->row() - 1) / 2}//{10,10,-6.3f}
		});

	playerManager = std::make_unique<PlayerManager>();
	playerManager->initialize(levelLoader, fieldObjs.get(), fieldObjs->GetGoalPos());

	tutorialManager = std::make_unique<TutorialManager>();
	tutorialManager->initialize(currentLevel);
	playerManager->set_tutorial_manager(tutorialManager.get());

	transition = eps::CreateUnique<SpriteInstance>("black.png");

	directionalLight = eps::CreateUnique<DirectionalLightInstance>();

	std::shared_ptr<SingleRenderTarget> meshRT;
	meshRT = std::make_shared<SingleRenderTarget>();
	meshRT->initialize();

	Particle::lookAtDefault = camera3D.get();

	std::shared_ptr<SpriteNode> bgSpriteNode;
	bgSpriteNode = std::make_unique<SpriteNode>();
	bgSpriteNode->initialize();
	bgSpriteNode->set_config(
		RenderNodeConfig::ContinueDrawBefore
	);
	bgSpriteNode->set_render_target(meshRT);

	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueDrawBefore | RenderNodeConfig::ContinueUseDpehtBefore);
	object3dNode->set_render_target(meshRT);

	std::shared_ptr<SkinningMeshNode> skinningMeshNode;
	skinningMeshNode = std::make_unique<SkinningMeshNode>();
	skinningMeshNode->initialize();
	skinningMeshNode->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueUseDpehtAfter);
	skinningMeshNode->set_render_target(meshRT);

	outlineNode = std::make_shared<OutlineNode>();
	outlineNode->initialize();
	outlineNode->set_config(RenderNodeConfig::ContinueDrawBefore);
	outlineNode->set_depth_resource(DepthStencilValue::depthStencil->texture_gpu_handle());
	outlineNode->set_texture_resource(meshRT->offscreen_render().texture_gpu_handle());
	outlineNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	std::shared_ptr<ParticleMeshNode> particleMeshNode;
	particleMeshNode = std::make_unique<ParticleMeshNode>();
	particleMeshNode->initialize();
	//particleBillboardNode->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueUseDpehtBefore);
	particleMeshNode->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::NoClearDepth | RenderNodeConfig::ContinueDrawBefore);
	//particleBillboardNode->set_render_target(renderTarget);s
	particleMeshNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(
		RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueDrawBefore
	);
	spriteNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

#ifdef _DEBUG
	std::shared_ptr<PrimitiveLineNode> primitiveLineNode;
	primitiveLineNode = std::make_unique<PrimitiveLineNode>();
	primitiveLineNode->initialize();
#endif // _DEBUG

	// ---------------------- ParticleBillboard ----------------------


	renderPath = eps::CreateUnique<RenderPath>();
#ifdef _DEBUG
	renderPath->initialize({ bgSpriteNode,object3dNode,skinningMeshNode,outlineNode,particleMeshNode,spriteNode,primitiveLineNode });
#else
	renderPath->initialize({ bgSpriteNode,object3dNode,skinningMeshNode,outlineNode,particleMeshNode,spriteNode });
#endif // _DEBUG

	managementUI = std::make_unique<GameManagement>();
	playerManager->set_game_management(managementUI.get());
	managementUI->SetMaxLevel(GameValue::MaxLevel);
	managementUI->SetCurLevel(currentLevel);
	gameUI = std::make_unique<GameSceneUI>();
	gameUI->initialize(currentLevel);

	tutorialManager->set_game_management(managementUI.get());

	bgm = std::make_unique<AudioPlayer>();
	bgm->initialize("Game.wav");
	bgm->set_loop(true);
	bgm->set_volume(0.1f);
	bgm->play();

	background = std::make_unique<BackGround>();
	rocketObj = std::make_unique<Rocket>(fieldObjs->GetGoalPos());
	Input::SetDeadZone(0.6f);
}

void GameScene::popped() {}

void GameScene::finalize() {}

void GameScene::begin() {
	GameValue::UiType.update();
	if (playerManager->get_player_state() == PlayerState::Idle||
		managementUI->GetClearFlag()|| managementUI->GetFailedFlag()
		) {
		managementUI->begin();
	}

	managementUI->SetCurLevel(currentLevel);
	gameUI->SetCurLevel(currentLevel);

}

void GameScene::update() {
	playerManager->update();
	fieldObjs->update();
	directionalLight->update();
	managementUI->update();
	gameUI->update();
	background->update();
	gameUI->setIsCanRelese(playerManager->get_isParent());
	rocketObj->update(playerManager->getStageSituation());
	tutorialManager->update();
}

void GameScene::begin_rendering() {
	playerManager->begin_rendering();
	fieldObjs->begin_rendering();

	camera3D->update_matrix();
	directionalLight->begin_rendering();
	managementUI->begin_rendering();
	gameUI->begin_rendering();
	background->begin_rendering();
	rocketObj->begin_rendering();

	transition->begin_rendering();
	tutorialManager->begin_rendering();
}

void GameScene::late_update() {
	if (managementUI->is_reset()) {
		fieldObjs->initialize(levelLoader);
		playerManager->initialize(levelLoader, fieldObjs.get(), fieldObjs->GetGoalPos(), false);
		managementUI->init();
		rocketObj->init();
	}
	else if (managementUI->is_undoRestart()) {
		//fieldObjs->initialize(levelLoader);
		//playerManager->initialize(levelLoader, fieldObjs.get());
		managementUI->init();
		rocketObj->init();
		//ここで一手戻す処理をする
		playerManager->restart_undo();
	}
	else if (managementUI->is_escape_game() && sceneState != TransitionState::Out) {
		SceneManager::SetSceneChange(
			eps::CreateUnique<SelectScene>(currentLevel), sceneChangeTime
		);
		transitionTimer = WorldClock::DeltaSeconds();
		sceneState = TransitionState::Out;
	}
	else if (managementUI->is_next() && sceneState != TransitionState::Out) {
		//managementUI->init();
		// 最大レベルではない場合
		if (currentLevel < GameValue::MaxLevel) {
			SceneManager::SetSceneChange(
				eps::CreateUnique<GameScene>(currentLevel + 1), sceneChangeTime
			);
			transitionTimer = WorldClock::DeltaSeconds();
			sceneState = TransitionState::Out;
		}
		// 最大レベルの場合
		else {
			// TODO : ここに最大レベル時の遷移を実装する
			SceneManager::SetSceneChange(
			eps::CreateUnique<TitleScene>(), sceneChangeTime);
		}
		transitionTimer = WorldClock::DeltaSeconds();
		sceneState = TransitionState::Out;
	}

	switch (sceneState) {
	case TransitionState::In:
	{
		transitionTimer += WorldClock::DeltaSeconds();
		float parametric = std::min(1.0f, transitionTimer / sceneChangeTime);
		transition->get_color().alpha = 1 - parametric;
		if (parametric >= 1.0f) {
			sceneState = TransitionState::Main;
		}
		break;
	}
	case TransitionState::Main:
		if (managementUI->is_transition()) {
			transitionTimer = WorldClock::DeltaSeconds();
			sceneState = TransitionState::Out;
		}
		break;
	case TransitionState::Out:
	{
		transitionTimer += WorldClock::DeltaSeconds();
		float parametric = std::min(1.0f, transitionTimer / sceneChangeTime);
		transition->get_color().alpha = parametric;
		if (!managementUI->is_restart()) {
			bgm->set_volume((1 - parametric) * 0.1f);
		}
		if (parametric >= 1.0f) {
			// リセット処理をここで呼び出す
			if (managementUI->is_restart()) {
				sceneState = TransitionState::In;
				fieldObjs->initialize(levelLoader);
				playerManager->initialize(levelLoader, fieldObjs.get(), fieldObjs->GetGoalPos());
				managementUI->init();
				rocketObj->init();
				transitionTimer = WorldClock::DeltaSeconds();
			}
			//managementUI->init();
		}
		break;
	}
	}
}

void GameScene::draw() const {
	// 背景スプライト
	renderPath->begin();
	background->draw();

	// StaticMesh
	renderPath->next();
	camera3D->register_world_projection(1);
	camera3D->register_world_lighting(4);
	directionalLight->register_world(5);

	fieldObjs->draw();

#ifdef _DEBUG
	camera3D->debug_draw_axis();
#endif // _DEBUG

	// SkinningMesh
	renderPath->next();
	camera3D->register_world_projection(1);
	camera3D->register_world_lighting(5);
	directionalLight->register_world(6);
	playerManager->draw();
	rocketObj->draw();
	background->animeDraw();
	// Outline
	renderPath->next();
	outlineNode->draw();

	// 前景スプライト
	renderPath->next();
	camera3D->register_world_projection(1);
	playerManager->draw_particle();
	rocketObj->draw_particle();
	background->drawParticle();

	renderPath->next();
	gameUI->darw();
	tutorialManager->draw();
	managementUI->darw();

	playerManager->draw_sprite();


	transition->draw();


	renderPath->next();

#ifdef _DEBUG
	// 線描画(Debug)
	camera3D->register_world_projection(1);
	camera3D->debug_draw_frustum();

	renderPath->next();
#endif // _DEBUG
}

void GameScene::reset_level() {}

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
	rocketObj->debug_update();
	background->debugUpdate();
	//ImGui::Begin("OutlineNode");
	//outlineNode->
	//ImGui::End();
	tutorialManager->debug_update();


	AudioManager::DebugGui();
}
#endif // _DEBUG
