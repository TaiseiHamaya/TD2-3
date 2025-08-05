#include "SelectScene.h"

#include <Library/Math/Definition.h>
#include <Library/Utility/Tools/Easing.h>

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
#include <Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h>
#include <Engine/Module/World/Camera/Camera2D.h>
#include <Engine/Module/World/Mesh/SkinningMeshInstance.h>
#include <Engine/Module/World/Mesh/StaticMeshInstance.h> 
#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Library/Utility/Tools/SmartPointer.h>

#include "Application/Scene/TitleScene.h"

#include "Application/GameValue.h"
#include "Application/LevelLoader/LevelLoader.h"
#include "Application/Scene/GameScene.h"
#include <Application/Configuration/Configuration.h>

#include "Application/PostEffect/BloomNode.h"
#include "Application/PostEffect/GaussianBlurNode.h"
#include "Application/PostEffect/LuminanceExtractionNode.h"
#include "Application/PostEffect/MargeTextureNode.h"

SelectScene::SelectScene() : SelectScene(1, false) {};

SelectScene::SelectScene(int32_t selectLevel, bool isFromGame) :
	selectIndex(selectLevel) {
	isFromGameScene = isFromGame;
}

SelectScene::~SelectScene() = default;

void SelectScene::load() {
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/RordObj/RordObj.obj");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/WallObj/WallObj.obj");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.obj");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/IceObj/IceObj.obj");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObjStatic.obj");

	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/StageSelectUI.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/StartController.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/ESCkeyController.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/start.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/number.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/KoaraFace.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/ResetBack.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/obi.png");

	AudioLibrary::RegisterLoadQue("./GameResources/Audio/BGM/Title.wav");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/backGround.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/backGround2.png");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/BGM/SelectBGM.wav");

	AudioLibrary::RegisterLoadQue("./GameResources/Audio/change.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/stageStart.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/backAudio.wav");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/ESCkey.png");

	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");

}

void SelectScene::initialize() {
	sceneState = TransitionState::Main;

	// Instance
	Camera2D::Initialize();
	camera3D = std::make_unique<Camera3D>();
	camera3D->initialize();

	directionalLight = eps::CreateUnique<DirectionalLightInstance>();

	goalMesh = eps::CreateUnique<StaticMeshInstance>("GoalObjStatic.obj");
	parentKoala = eps::CreateUnique<SkinningMeshInstance>("ParentKoala.gltf", "Standby", true);
	childKoala = eps::CreateUnique<SkinningMeshInstance>("ChiledKoala.gltf", "Standby", true);

	switch (Configuration::GetLanguage()) {
	case Configuration::Language::Japanese:
		startUi[0] = eps::CreateUnique<SpriteInstance>("StartController.png", Vector2{ 0.5f, 0.5f });
		startUi[1] = eps::CreateUnique<SpriteInstance>("start.png", Vector2{ 0.5f, 0.5f });
		break;
	case Configuration::Language::English:
		startUi[0] = eps::CreateUnique<SpriteInstance>("StartController_EN.png", Vector2{ 0.5f, 0.5f });
		startUi[1] = eps::CreateUnique<SpriteInstance>("start_EN.png", Vector2{ 0.5f, 0.5f });
		break;
	}
	startUi[0]->get_transform().set_translate({ 640.0f,90 });
	startUi[0]->set_priority(2);
	startUi[1]->get_transform().set_translate({ 640.0f,90 });
	startUi[1]->set_priority(2);
	selectUi = eps::CreateUnique<SpriteInstance>("StageSelectUI.png", Vector2{ 0.5f, 0.5f });
	selectUi->get_transform().set_translate({ 640.0f,650.0f });
	selectUi->set_priority(2);
	numberUi = eps::CreateUnique<SpriteInstance>("number.png", Vector2{ 0.5f, 0.5f });
	numberUi->get_transform().set_scale({ 0.1f,1.0f });
	numberUi->get_material().uvTransform.set_scale({ 0.1f,1.0f });
	numberUi->set_priority(2);
	numberUi10 = eps::CreateUnique<SpriteInstance>("number.png", Vector2{ 0.5f, 0.5f });
	numberUi10->get_transform().set_translate({ 640.0f - 96 / 2,360.0f });
	numberUi10->get_transform().set_scale({ 0.1f,1.0f });
	numberUi10->get_material().uvTransform.set_scale({ 0.1f,1.0f });
	numberUi10->set_priority(2);
	if (selectIndex < 10) {
		numberUi10->set_active(false);
	}
	obSprite = std::make_unique<SpriteInstance>("obi.png");
	obSprite->set_priority(3);
	transition = eps::CreateUnique<SpriteInstance>("black.png");

	backTitleSprite[0] = std::make_unique<SpriteInstance>("ESCkeyController.png");
	backTitleSprite[0]->get_transform().set_scale({ 0.5f, 1.0f });
	backTitleSprite[0]->get_material().uvTransform.set_scale({ 0.5f, 1.0f });
	backTitleSprite[0]->get_transform().set_translate({ 1141,30 });
	backTitleSprite[0]->set_priority(2);
	backTitleSprite[1] = std::make_unique<SpriteInstance>("ESCkey.png");
	backTitleSprite[1]->get_transform().set_scale({ 0.5f, 1.0f });
	backTitleSprite[1]->get_material().uvTransform.set_scale({ 0.5f, 1.0f });
	backTitleSprite[1]->get_transform().set_translate({ 1141,30 });
	backTitleSprite[1]->set_priority(2);

	fromGameBack = std::make_unique<SpriteInstance>("ResetBack.png", Vector2(0.5f, 0.5f));
	fromGameBack->get_transform().set_translate({ 640.0f, -360.0f });
	fromGameBack->set_priority(1);

	fromGameKoara = std::make_unique<SpriteInstance>("KoaraFace.png", Vector2(0.5f, 0.5f));
	fromGameKoara->get_transform().set_translate({ 640.0f, -360.0f });
	fromGameBack->set_priority(0);

	LevelLoader loader{ selectIndex };

	field = std::make_unique<MapchipField>();

	fieldRotation = std::make_unique<WorldInstance>();

	Particle::lookAtDefault = camera3D.get();

	renderTextures.resize(8);
	renderTextures[0].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB); // アウトライン前
	renderTextures[1].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB); // シーンアウト
	renderTextures[2].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB); // 輝度抽出
	renderTextures[3].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, EngineSettings::CLIENT_WIDTH / 2, EngineSettings::CLIENT_HEIGHT / 2); // ダウンサンプリング 1/2
	renderTextures[4].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, EngineSettings::CLIENT_WIDTH / 4, EngineSettings::CLIENT_HEIGHT / 4); // ダウンサンプリング 1/4
	renderTextures[5].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, EngineSettings::CLIENT_WIDTH / 8, EngineSettings::CLIENT_HEIGHT / 8); // ダウンサンプリング 1/8
	renderTextures[6].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, EngineSettings::CLIENT_WIDTH / 16, EngineSettings::CLIENT_HEIGHT / 16); // ダウンサンプリング 1/16
	renderTextures[7].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB); // ダウンサンプリングを合成

	meshRT.initialize(renderTextures[0]);
	baseRenderTexture.initialize(renderTextures[1]);
	luminanceRenderTexture.initialize(renderTextures[2]);
	downSampleRenderTexture2.initialize(renderTextures[3]);
	downSampleRenderTexture4.initialize(renderTextures[4]);
	downSampleRenderTexture8.initialize(renderTextures[5]);
	downSampleRenderTexture16.initialize(renderTextures[6]);
	bloomBaseRenderTexture.initialize(renderTextures[7]);

	std::shared_ptr<SpriteNode> bgSpriteNode;
	bgSpriteNode = std::make_unique<SpriteNode>();
	bgSpriteNode->initialize();
	bgSpriteNode->set_render_target(meshRT);

	std::shared_ptr<StaticMeshNodeForward> object3dNode;
	object3dNode = std::make_unique<StaticMeshNodeForward>();
	object3dNode->initialize();
	object3dNode->set_config(RenderNodeConfig::NoClearRenderTarget);
	object3dNode->set_render_target(meshRT);

	std::shared_ptr<SkinningMeshNodeForward> skinningMeshNode;
	skinningMeshNode = std::make_unique<SkinningMeshNodeForward>();
	skinningMeshNode->initialize();
	skinningMeshNode->set_config(RenderNodeConfig::NoClearRenderTarget | RenderNodeConfig::NoClearDepth);
	skinningMeshNode->set_render_target(meshRT);

	outlineNode = std::make_shared<OutlineNode>();
	outlineNode->initialize();
	outlineNode->set_shader_texture(renderTextures[0], RenderingSystemValues::GetDepthStencilTexture());
	outlineNode->set_render_target(baseRenderTexture);

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(RenderNodeConfig::NoClearRenderTarget);
	spriteNode->set_render_target(baseRenderTexture);

	luminanceExtractionNode = eps::CreateShared<LuminanceExtractionNode>();
	luminanceExtractionNode->initialize();
	luminanceExtractionNode->set_render_target(luminanceRenderTexture);
	luminanceExtractionNode->set_texture_resource(renderTextures[1]);

	gaussianBlurNode2 = eps::CreateShared<GaussianBlurNode>();
	gaussianBlurNode2->initialize();
	gaussianBlurNode2->set_render_target(downSampleRenderTexture2);
	gaussianBlurNode2->set_base_texture(renderTextures[2]);

	gaussianBlurNode4 = eps::CreateShared<GaussianBlurNode>();
	gaussianBlurNode4->initialize();
	gaussianBlurNode4->set_render_target(downSampleRenderTexture4);
	gaussianBlurNode4->set_base_texture(renderTextures[3]);

	gaussianBlurNode8 = eps::CreateShared<GaussianBlurNode>();
	gaussianBlurNode8->initialize();
	gaussianBlurNode8->set_render_target(downSampleRenderTexture8);
	gaussianBlurNode8->set_base_texture(renderTextures[4]);

	gaussianBlurNode16 = eps::CreateShared<GaussianBlurNode>();
	gaussianBlurNode16->initialize();
	gaussianBlurNode16->set_render_target(downSampleRenderTexture16);
	gaussianBlurNode16->set_base_texture(renderTextures[5]);

	margeTextureNode = eps::CreateShared<MargeTextureNode>();
	margeTextureNode->initialize();
	margeTextureNode->set_render_target(bloomBaseRenderTexture);
	margeTextureNode->set_texture_resources({ renderTextures[3] ,renderTextures[4] ,renderTextures[5], renderTextures[6] });

	bloomNode = eps::CreateShared<BloomNode>();
	bloomNode->initialize();
	bloomNode->set_render_target_SC();
	bloomNode->set_base_texture(renderTextures[1]);
	bloomNode->set_blur_texture(renderTextures[7]);

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({ bgSpriteNode,object3dNode,skinningMeshNode,outlineNode,spriteNode,
		luminanceExtractionNode, gaussianBlurNode2, gaussianBlurNode4, gaussianBlurNode8, gaussianBlurNode16, margeTextureNode, bloomNode });

	// ---------------------- DrawManager ----------------------
	staticMeshDrawManager = std::make_unique<StaticMeshDrawManager>();
	staticMeshDrawManager->initialize(1);
	staticMeshDrawManager->make_instancing(0, "RordObj.obj", 256);
	staticMeshDrawManager->make_instancing(0, "WallObj.obj", 256);
	staticMeshDrawManager->make_instancing(0, "IceObj.obj", 256);
	staticMeshDrawManager->make_instancing(0, "GoalObjStatic.obj", 1);
	skinningMeshDrawManager = std::make_unique<SkinningMeshDrawManager>();
	skinningMeshDrawManager->initialize(1);
	skinningMeshDrawManager->make_instancing(0, "ParentKoala.gltf", 1);
	skinningMeshDrawManager->make_instancing(0, "ChiledKoala.gltf", 1);
	bgSpriteDrawExecutor = std::make_unique<SpriteDrawExecutor>();
	bgSpriteDrawExecutor->reinitialize(16);
	spriteDrawExecutors.resize(4);
	spriteDrawExecutors[0] = std::make_unique<SpriteDrawExecutor>();
	spriteDrawExecutors[0]->reinitialize(2);
	spriteDrawExecutors[1] = std::make_unique<SpriteDrawExecutor>();
	spriteDrawExecutors[1]->reinitialize(1);
	spriteDrawExecutors[2] = std::make_unique<SpriteDrawExecutor>();
	spriteDrawExecutors[2]->reinitialize(256);
	spriteDrawExecutors[2] = std::make_unique<SpriteDrawExecutor>();
	spriteDrawExecutors[2]->reinitialize(16);

	directionalLightingExecutor = std::make_unique<DirectionalLightingExecutor>();
	directionalLightingExecutor->reinitialize(1);

	bgm = std::make_unique<AudioPlayer>();
	bgm->initialize("SelectBGM.wav");
	bgm->set_loop(true);
	bgm->set_volume(0.1f);
	bgm->play();

	background = std::make_unique<BackGround>();
	fadeEase = 1;

	startAudio = std::make_unique<AudioPlayer>();
	startAudio->initialize("stageStart.wav");
	selectAudio = std::make_unique<AudioPlayer>();
	selectAudio->initialize("change.wav");
	selectAudio->set_volume(0.5f);
	backTitle = std::make_unique<AudioPlayer>();
	backTitle->initialize("backAudio.wav");
	backTitle->set_volume(GameValue::SelectBgmVolume);
	//}

	// 入力遅延時間
	InputDowntime = 0.3f;
	inputTimer = InputDowntime;

	crate_field_view();

	skinningMeshDrawManager->register_instance(parentKoala);
	skinningMeshDrawManager->register_instance(childKoala);
	staticMeshDrawManager->register_instance(goalMesh);

	luminanceExtractionNode->set_param(0.67f, CColor3::WHITE);
	gaussianBlurNode2->set_parameters(1.0f, 16.17f, 8);
	gaussianBlurNode4->set_parameters(1.0f, 16.17f, 8);
	gaussianBlurNode8->set_parameters(1.0f, 16.17f, 8);
	gaussianBlurNode16->set_parameters(1.0f, 16.17f, 8);
	bloomNode->set_param(0.247f);
}

void SelectScene::popped() {
}

void SelectScene::finalize() {
}

void SelectScene::begin() {
	if (Input::IsTriggerKey(KeyID::One)) {
		float masterVolume = AudioManager::GetMasterVolume();
		AudioManager::SetMasterVolume(masterVolume - 0.1f);
	}
	else if (Input::IsTriggerKey(KeyID::Two)) {
		float masterVolume = AudioManager::GetMasterVolume();
		AudioManager::SetMasterVolume(masterVolume + 0.1f);
	}
	if (Input::IsTriggerKey(KeyID::Three)) {
		GameValue::SelectBgmVolume -= 0.1f;
		bgm->set_volume(GameValue::SelectBgmVolume);
	}
	else if (Input::IsTriggerKey(KeyID::Four)) {
		GameValue::SelectBgmVolume += 0.1f;
		bgm->set_volume(GameValue::SelectBgmVolume);
	}
	GameValue::UiType.update();
	parentKoala->begin();
	childKoala->begin();

	inputTimer -= WorldClock::DeltaSeconds();
}

void SelectScene::update() {
	switch (sceneState) {
	case SelectScene::TransitionState::Main:
		default_update();
		break;
	case SelectScene::TransitionState::Out:
		out_update();
		break;
	}

	// 2桁表示
	if (selectIndex >= 10) {
		numberUi->get_transform().set_translate({ 640.0f + 96 / 2,360.0f });
		numberUi10->set_active(true);
	}
	// 1桁表示
	else {
		numberUi->get_transform().set_translate({ 640.0f,360.0f });
		numberUi10->set_active(false);
	}

	numberUi->get_material().uvTransform.set_translate_x(selectIndex * 0.1f);
	numberUi10->get_material().uvTransform.set_translate_x((selectIndex / 10) * 0.1f);

	Quaternion rotation = fieldRotation->get_transform().get_quaternion();
	fieldRotation->get_transform().set_quaternion(Quaternion::AngleAxis(CVector3::BASIS_Y, PI2 / 3 * WorldClock::DeltaSeconds()) * rotation);

	background->update();

	from_game_update();
}

void SelectScene::begin_rendering() {
	directionalLightingExecutor->begin();
	bgSpriteDrawExecutor->begin();
	for (auto& executor : spriteDrawExecutors) {
		executor->begin();
	}

	camera3D->update_affine();
	fieldRotation->update_affine();
	field->update_affine();
	goalMesh->update_affine();
	parentKoala->update_affine();
	childKoala->update_affine();

	parentKoala->update_animation();
	childKoala->update_animation();

	camera3D->transfer();

	staticMeshDrawManager->transfer();
	skinningMeshDrawManager->transfer();
	directionalLightingExecutor->write_to_buffer(directionalLight);

	spriteDrawExecutors[0]->write_to_buffer(transition);
	spriteDrawExecutors[0]->write_to_buffer(fromGameBack);
	spriteDrawExecutors[1]->write_to_buffer(fromGameKoara);
	spriteDrawExecutors[2]->write_to_buffer(numberUi);
	spriteDrawExecutors[2]->write_to_buffer(numberUi10);
	spriteDrawExecutors[2]->write_to_buffer(selectUi);
	spriteDrawExecutors[2]->write_to_buffer(startUi[(int)GameValue::UiType.get_type()]);
	spriteDrawExecutors[2]->write_to_buffer(backTitleSprite[(int)GameValue::UiType.get_type()]);
	spriteDrawExecutors[3]->write_to_buffer(obSprite);
	background->write_to_executor(bgSpriteDrawExecutor);
}

void SelectScene::late_update() {
}

void SelectScene::draw() const {
	renderPath->begin();
	// 背景スプライト
	bgSpriteDrawExecutor->draw_command();

	renderPath->next();
	// Mesh
	camera3D->register_world_projection(2);
	camera3D->register_world_lighting(3);
	directionalLightingExecutor->set_command(4);
	staticMeshDrawManager->draw_layer(0);

	renderPath->next();
	// SkinningMesh
	camera3D->register_world_projection(2);
	camera3D->register_world_lighting(5);
	directionalLightingExecutor->set_command(6);
	skinningMeshDrawManager->draw_layer(0);

	renderPath->next();
	outlineNode->draw();

	renderPath->next();
	// 前景スプライト
	for (auto& executor : spriteDrawExecutors | std::views::reverse) {
		executor->draw_command();
	}

	renderPath->next();
	luminanceExtractionNode->draw();
	renderPath->next();
	gaussianBlurNode2->draw();
	renderPath->next();
	gaussianBlurNode4->draw();
	renderPath->next();
	gaussianBlurNode8->draw();
	renderPath->next();
	gaussianBlurNode16->draw();
	renderPath->next();
	margeTextureNode->draw();
	renderPath->next();
	bloomNode->draw();

	renderPath->next();
}

void SelectScene::crate_field_view() {
	fieldRotation->get_transform().set_quaternion(CQuaternion::IDENTITY);

	LevelLoader loader{ selectIndex };
	field->initialize(loader);
	Reference<WorldInstance> fieldRoot = field->field_root();

	fieldRotation->get_transform().set_translate(
		{ static_cast<float>(field->column() - 1) / 2,0,static_cast<float>(field->row() - 1) / 2 }
	);
	fieldRotation->update_affine();
	fieldRoot->reparent(fieldRotation, true);

	camera3D->set_transform({
		CVector3::BASIS,
		 Quaternion::EulerDegree(40,0,0),
		{ static_cast<float>(field->column() - 1) / 2,10,-12 + static_cast<float>(field->row() - 1) / 2}
		});

	goalMesh->reparent(fieldRoot, false);
	goalMesh->get_transform().set_translate(field->GetGoalPos());
	parentKoala->reparent(fieldRoot, false);
	parentKoala->get_transform().set_translate(loader.get_player_position());
	parentKoala->update_affine();
	childKoala->reparent(fieldRoot, false);
	childKoala->get_transform().set_translate(loader.get_child_position());
	childKoala->look_at(*parentKoala);
	auto& rotation = childKoala->get_transform().get_quaternion();
	childKoala->get_transform().set_quaternion(
		Quaternion::AngleAxis(CVector3::BASIS_Y, PI) * rotation
	);
	field->setup(staticMeshDrawManager);
}

void SelectScene::default_update() {
	//フェード処理
	transitionTimer += WorldClock::DeltaSeconds();
	if (fadeEase >= 0) {
		fadeEase -= WorldClock::DeltaSeconds();
		float fadeRatio = std::max(0.f, fadeEase / 1.0f);
		transition->get_material().color.alpha = fadeEase;
	}

	Vector2 stickL = Input::StickL().normalize_safe(CVector2::ZERO);
	if (inputTimer <= 0.0f) {
		if (Input::IsPressKey(KeyID::D) || Input::IsPressKey(KeyID::Right) ||
			Input::IsPressPad(PadID::Right) || stickL.x > 0.5f) {
			++selectIndex;
			selectIndex = (selectIndex - 1) % GameValue::MaxLevel + 1;
			crate_field_view();
			selectAudio->restart();
			inputTimer = InputDowntime;
		}
		else if (Input::IsPressKey(KeyID::A) || Input::IsPressKey(KeyID::Left) ||
			Input::IsPressPad(PadID::Left) || stickL.x < -0.5f) {
			--selectIndex;
			selectIndex = (selectIndex - 1) % GameValue::MaxLevel + 1;
			if (selectIndex <= 0) {
				selectIndex += GameValue::MaxLevel;
			}
			crate_field_view();
			selectAudio->restart();
			inputTimer = InputDowntime;
		}
	}
	else {
		// キーを離したら強制的にInputDownTimerを0にする
		if (!(Input::IsPressKey(KeyID::D) || Input::IsPressKey(KeyID::Right) ||
			Input::IsPressPad(PadID::Right) ||
			Input::IsPressKey(KeyID::A) || Input::IsPressKey(KeyID::Left) ||
			Input::IsPressPad(PadID::Left) || stickL.x != 0)
			) {
			inputTimer = 0;
		}
	}
	if (Input::IsTriggerKey(KeyID::Escape) || Input::IsTriggerPad(PadID::Start)) {
		SceneManager::SetSceneChange(
			eps::CreateUnique<TitleScene>(), 0.5f);
		backTitle->play();

		backTitleSprite[0]->get_material().uvTransform.set_translate_x(0.5f);
		backTitleSprite[1]->get_material().uvTransform.set_translate_x(0.5f);
	}
	// 2桁表示
	if (selectIndex >= 10) {
		numberUi->get_transform().set_translate({ 640.0f + 96 / 2,360.0f });
		numberUi10->set_active(true);
	}
	// 1桁表示
	else {
		numberUi->get_transform().set_translate({ 640.0f,360.0f });
		numberUi10->set_active(false);
	}

	if (Input::IsTriggerKey(KeyID::Space) || Input::IsTriggerPad(PadID::A)) {
		SceneManager::SetSceneChange(
			eps::CreateUnique<GameScene>(selectIndex), 1.f
		);
		sceneState = TransitionState::Out;
		startRotation = fieldRotation->get_transform().get_quaternion();
		transitionTimer = WorldClock::DeltaSeconds();
		startAudio->play();
	}
}

void SelectScene::out_update() {
	transitionTimer += WorldClock::DeltaSeconds();
	fadeEase += WorldClock::DeltaSeconds();
	float parametric = std::min(1.0f, transitionTimer / 1.0f);
	if (parametric >= 1.0f) {
		fadeEase = 1.0f;
	}
	float fadeRatio = std::min(1.f, fadeEase);
	fieldRotation->get_transform().set_quaternion(
		Quaternion::SlerpFar(startRotation,
			Quaternion::AngleAxis(CVector3::BASIS_Y, -0.01f) * startRotation,
			Easing::Out::Quad(parametric))
	);
	transition->get_material().color.alpha = fadeRatio;
	bgm->set_volume((1 - fadeEase) * 0.1f);
}

void SelectScene::from_game_update() {
	if (isFromGameScene) {
		fromGameBack->set_active(true);
		fromGameKoara->set_active(true);
	}
	else {
		fromGameBack->set_active(false);
		fromGameKoara->set_active(false);
	}

	if (fromGameCurrentTime >= fromGameMaxTime) {
		//toSelectState = ResetState::Idle;
		fromGameCurrentTime = 1.0f;
	}
	else {
		fromGameCurrentTime += WorldClock::DeltaSeconds();

		float t = std::clamp(fromGameCurrentTime / fromGameMaxTime, 0.0f, 1.0f);
		float newPos = 360.0f + (1080.0f + -360.0f) * t;
		fromGameBack->get_transform().set_translate_y(newPos);
		fromGameKoara->get_transform().set_translate_y(newPos);
	}
}

#ifdef _DEBUG
void SelectScene::debug_update() {
	ImGui::Begin("SelectUi");
	selectUi->debug_gui();
	ImGui::End();
	ImGui::Begin("startUi");
	startUi[1]->debug_gui();
	ImGui::End();


	ImGui::Begin("Camera");
	camera3D->debug_gui();
	ImGui::End();

	ImGui::Begin("WorldClock");
	WorldClock::DebugGui();
	ImGui::End();
	background->debugUpdate();

	ImGui::Begin("PostEffect");
	if (ImGui::TreeNode("LuminanceExtraction")) {
		luminanceExtractionNode->debug_gui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("GaussianBlurNode16")) {
		ImGui::DragFloat("Weight", &blurData.dispersion, 0.001f, 0.0f, 1.0f, "%.4f");
		ImGui::DragFloat("Length", &blurData.length, 0.01f);
		constexpr uint32_t min = 1;
		constexpr uint32_t max = 16;
		ImGui::DragScalar("SampleCount", ImGuiDataType_U32, reinterpret_cast<int*>(&blurData.sampleCount), 0.02f, &min, &max);

		gaussianBlurNode2->set_parameters(blurData.dispersion, blurData.length, blurData.sampleCount);
		gaussianBlurNode4->set_parameters(blurData.dispersion, blurData.length, blurData.sampleCount);
		gaussianBlurNode8->set_parameters(blurData.dispersion, blurData.length, blurData.sampleCount);
		gaussianBlurNode16->set_parameters(blurData.dispersion, blurData.length, blurData.sampleCount);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("BloomNode")) {
		bloomNode->debug_gui();
		ImGui::TreePop();
	}
	ImGui::End();
}
#endif // _DEBUG
