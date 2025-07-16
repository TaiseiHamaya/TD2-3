#include "GameScene.h"

//#include <Engine/Rendering/DirectX/DirectXResourceObject/DepthStencil/DepthStencil.h>
//#include <Engine/Rendering/DirectX/DirectXResourceObject/OffscreenRender/OffscreenRender.h>
//#include <Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Assets/Animation/NodeAnimation/NodeAnimationLibrary.h>
#include <Engine/Assets/Animation/Skeleton/SkeletonLibrary.h>
#include <Engine/Assets/Audio/AudioLibrary.h>
#include <Engine/Assets/Audio/AudioManager.h>
#include <Engine/Assets/PolygonMesh/PolygonMeshLibrary.h>
#include <Engine/Assets/Texture/TextureLibrary.h>
#include <Engine/GraphicsAPI/RenderingSystemValues.h>
#include <Engine/Module/Render/RenderNode/2D/Sprite/SpriteNode.h>
#include <Engine/Module/Render/RenderNode/Forward/Mesh/SkinningMeshNodeForward.h>
#include <Engine/Module/Render/RenderNode/Forward/Mesh/StaticMeshNodeForward.h>
#include <Engine/Module/Render/RenderNode/Forward/Particle/ParticleMeshNode/ParticleMeshNode.h>
#include <Engine/Module/Render/RenderNode/Posteffect/Outline/OutlineNode.h>
#include <Engine/Module/Render/RenderPath/RenderPath.h>
#include <Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h>
#include <Engine/Module/World/Camera/Camera2D.h>
#include <Engine/Module/World/Camera/Camera3D.h>
#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Runtime/Clock/WorldClock.h>
#include <Engine/Runtime/Scene/SceneManager.h>

#include "Application/GameValue.h"
#include "Application/LevelLoader/LevelLoader.h"
#include "Application/Scene/SelectScene.h"
#include "Application/Scene/TitleScene.h"

#include <Library/Utility/Tools/SmartPointer.h>

#ifdef _DEBUG
#include "Engine/Module/Render/RenderNode/Debug/PrimitiveLine/PrimitiveLineNode.h"
#endif // _DEBUG
	
GameScene::GameScene() : GameScene(1) {}

GameScene::GameScene(int32_t level) :
	currentLevel(level) {
}
;

GameScene::~GameScene() = default;

void GameScene::load() {
	//PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/Primitive/Sphere.obj");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/CatchEffect/CatchEffect.gltf");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/ReleaseEffect/ReleaseEffect.gltf");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/exclamation/exclamation.gltf");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/FlusteredEffect/FlusteredEffect.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/CatchEffect/CatchEffect.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/ReleaseEffect/ReleaseEffect.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/exclamation/exclamation.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/FlusteredEffect/FlusteredEffect.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/CatchEffect/CatchEffect.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/ReleaseEffect/ReleaseEffect.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/exclamation/exclamation.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/FlusteredEffect/FlusteredEffect.gltf");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/ClearTex.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/FailedTex.png");

	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");


	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/Wkey.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/Akey.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/Skey.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/Dkey.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/ResetUI.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/ESCkey.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/ReleseUI_EN.png");

	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/ResetUIController.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/ESCkeyController.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/UndoController.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/ReleseUIController.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/ReleseUIController_EN.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/NoneButton.png");

	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/Tutorial1.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/Next.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/Retry.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/SelectFrame.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/FailedUI_1.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/undoRetry.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/undoRetry_EN.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/GoSelect.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/Undo.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/smallNumber.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/stageFrame.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/backGround.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/backGround2.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Failed/Failed.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Failed/F.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Failed/a.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Failed/i.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Failed/l.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Failed/e.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Failed/d.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Failed/ten.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Clear/Clear.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Clear/C.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Clear/L.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Clear/E.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Clear/A.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Clear/R.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Clear/!.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Tutorial/Frame.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Tutorial/TutorialText.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Tutorial/TutorialText_EN.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/Tutorial/TutorialImage.png");

	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/Abutton.png");

	AudioLibrary::RegisterLoadQue("./GameResources/Audio/move.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/hold.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/release.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/fall.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/unmovable.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/iceMove.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/decision.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/operation.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/clearSound.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/failedSound.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/rotate.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/undo.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/explosion.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/gushing.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/reset.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/backAudio.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/BGM/Game.wav");

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

	renderTexture = std::make_unique<RenderTexture>();
	renderTexture->initialize(DXGI_FORMAT_R8G8B8A8_UNORM);

	std::shared_ptr<SingleRenderTarget> meshRT;
	meshRT = std::make_shared<SingleRenderTarget>();
	meshRT->initialize(renderTexture);

	Particle::lookAtDefault = camera3D.get();

	std::shared_ptr<SpriteNode> bgSpriteNode;
	bgSpriteNode = std::make_unique<SpriteNode>();
	bgSpriteNode->initialize();
	bgSpriteNode->set_render_target(meshRT.get());

	std::shared_ptr<StaticMeshNodeForward> object3dNode;
	object3dNode = std::make_unique<StaticMeshNodeForward>();
	object3dNode->initialize();
	object3dNode->set_config(RenderNodeConfig::NoClearRenderTarget);
	object3dNode->set_render_target(meshRT.get());

	std::shared_ptr<SkinningMeshNodeForward> skinningMeshNode;
	skinningMeshNode = std::make_unique<SkinningMeshNodeForward>();
	skinningMeshNode->initialize();
	skinningMeshNode->set_config(RenderNodeConfig::NoClearRenderTarget | RenderNodeConfig::NoClearDepth);
	skinningMeshNode->set_render_target(meshRT.get());

	outlineNode = std::make_shared<OutlineNode>();
	outlineNode->initialize();
	outlineNode->set_shader_texture(renderTexture, RenderingSystemValues::GetDepthStencilTexture());
	outlineNode->set_render_target_SC();

	std::shared_ptr<ParticleMeshNode> particleMeshNode;
	particleMeshNode = std::make_unique<ParticleMeshNode>();
	particleMeshNode->initialize();
	particleMeshNode->set_config(RenderNodeConfig::NoClearRenderTarget | RenderNodeConfig::NoClearDepth);
	particleMeshNode->set_render_target_SC();

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(RenderNodeConfig::NoClearRenderTarget);
	spriteNode->set_render_target_SC();

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

	// ---------------------- DrawManager ----------------------
	staticMeshDrawManager = std::make_unique<StaticMeshDrawManager>();
	staticMeshDrawManager->make_instancing(0, "RordObj.obj", 256);
	staticMeshDrawManager->make_instancing(0, "WallObj.obj", 256);
	staticMeshDrawManager->make_instancing(0, "IceObj.obj", 256);
	skinningMeshDrawManager = std::make_unique<SkinningMeshDrawManager>();
	skinningMeshDrawManager->make_instancing(0, "ParentKoala.gltf", 1);
	skinningMeshDrawManager->make_instancing(0, "ChiledKoala.gltf", 1);
	skinningMeshDrawManager->make_instancing(0, "CatchEffect.gltf", 1);
	skinningMeshDrawManager->make_instancing(0, "exclamation.gltf", 1);
	skinningMeshDrawManager->make_instancing(0, "FlusteredEffect.gltf", 1);
	skinningMeshDrawManager->make_instancing(0, "ReleaseEffect.gltf", 1);
	skinningMeshDrawManager->make_instancing(0, "CatchEffect.gltf", 1);
	skinningMeshDrawManager->make_instancing(0, "GoalObj.gltf", 1);
	bgSpriteDrawExecutor = std::make_unique<SpriteDrawExecutor>();
	bgSpriteDrawExecutor->reinitialize(16);
	spriteDrawExecutor = std::make_unique<SpriteDrawExecutor>();
	spriteDrawExecutor->reinitialize(256);

	directionalLightingExecutor = std::make_unique<DirectionalLightingExecutor>();
	directionalLightingExecutor->reinitialize(1);

	managementUI = std::make_unique<GameManagement>();
	playerManager->set_game_management(managementUI.get());
	managementUI->SetMaxLevel(GameValue::MaxLevel);
	managementUI->SetCurLevel(currentLevel);
	gameUI = std::make_unique<GameSceneUI>();
	gameUI->initialize(currentLevel);

	playerManager->setup(skinningMeshDrawManager);
	fieldObjs->setup(staticMeshDrawManager);

	tutorialManager->set_game_management(managementUI.get());

	bgm = std::make_unique<AudioPlayer>();
	bgm->initialize("Game.wav");
	bgm->set_loop(true);
	bgm->set_volume(GameValue::GameBgmVolume);
	bgm->play();

	background = std::make_unique<BackGround>();
	rocketObj = std::make_unique<Rocket>(fieldObjs->GetGoalPos());
	Input::SetDeadZone(0.6f);
}

void GameScene::popped() {}

void GameScene::finalize() {}

void GameScene::begin() {
	if (Input::IsTriggerKey(KeyID::One)) {
		float masterVolume = AudioManager::GetMasterVolume();
		AudioManager::SetMasterVolume(masterVolume - 0.1f);
	}
	else if (Input::IsTriggerKey(KeyID::Two)) {
		float masterVolume = AudioManager::GetMasterVolume();
		AudioManager::SetMasterVolume(masterVolume + 0.1f);
	}
	if (Input::IsTriggerKey(KeyID::Three)) {
		GameValue::GameBgmVolume -= 0.1f;
		bgm->set_volume(GameValue::GameBgmVolume);
	}
	else if (Input::IsTriggerKey(KeyID::Four)) {
		GameValue::GameBgmVolume += 0.1f;
		bgm->set_volume(GameValue::GameBgmVolume);
	}
	GameValue::UiType.update();
	if (playerManager->get_player_state() == PlayerState::Idle ||
		managementUI->GetClearFlag() || managementUI->GetFailedFlag()
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
	// Update affine
	directionalLight->update_affine();
	camera3D->update_affine();
	playerManager->update_affine();
	fieldObjs->update_affine();
	rocketObj->update_affine();
	
	// Transfer draw manager/executor
	skinningMeshDrawManager->transfer();
	staticMeshDrawManager->transfer();
	directionalLightingExecutor->write_to_buffer(directionalLight);

	// 2D
	bgSpriteDrawExecutor->begin();
	spriteDrawExecutor->begin();
	// Transfer
	background->write_to_executor(bgSpriteDrawExecutor);
	managementUI->write_to_executor(spriteDrawExecutor);
	gameUI->write_to_executor(spriteDrawExecutor);
	spriteDrawExecutor->write_to_buffer(transition);
	tutorialManager->write_to_executor(spriteDrawExecutor);
}

void GameScene::late_update() {

	if (managementUI->is_reset()) {
		tutorialManager->initialize(currentLevel);
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
			eps::CreateUnique<SelectScene>(currentLevel, true), sceneChangeTime
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
		transition->get_material().color.alpha = 1 - parametric;
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
		transition->get_material().color.alpha = parametric;
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
	bgSpriteDrawExecutor->draw_command();

	// StaticMesh
	renderPath->next();
	camera3D->register_world_projection(2);
	camera3D->register_world_lighting(3);
	directionalLightingExecutor->set_command(4);
	staticMeshDrawManager->draw_layer(0);

	//fieldObjs->draw();

	// SkinningMesh
	renderPath->next();
	camera3D->register_world_lighting(5);
	directionalLightingExecutor->set_command(6);
	skinningMeshDrawManager->draw_layer(0);

	// Outline
	renderPath->next();
	outlineNode->draw();

	// パーティクル
	renderPath->next();
	camera3D->register_world_projection(1);
	playerManager->draw_particle();
	rocketObj->draw_particle();
	background->drawParticle();

	// 全景スプライト
	renderPath->next();
	spriteDrawExecutor->draw_command();

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
	//rocketObj->debug_update();
	background->debugUpdate();
	//ImGui::Begin("OutlineNode");
	//outlineNode->
	//ImGui::End();
	tutorialManager->debug_update();


	AudioManager::DebugGui();
}
#endif // _DEBUG
