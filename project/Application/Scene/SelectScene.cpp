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
	startUi[1]->get_transform().set_translate({ 640.0f,90 });
	selectUi = eps::CreateUnique<SpriteInstance>("StageSelectUI.png", Vector2{ 0.5f, 0.5f });
	selectUi->get_transform().set_translate({ 640.0f,650.0f });
	numberUi = eps::CreateUnique<SpriteInstance>("number.png", Vector2{ 0.5f, 0.5f });
	numberUi->get_transform().set_scale({ 0.1f,1.0f });
	numberUi->get_uv_transform().set_scale({ 0.1f,1.0f });
	numberUi10 = eps::CreateUnique<SpriteInstance>("number.png", Vector2{ 0.5f, 0.5f });
	numberUi10->get_transform().set_translate({ 640.0f - 96 / 2,360.0f });
	numberUi10->get_transform().set_scale({ 0.1f,1.0f });
	numberUi10->get_uv_transform().set_scale({ 0.1f,1.0f });
	if (selectIndex < 10) {
		numberUi10->set_active(false);
	}
	obSprite = std::make_unique<SpriteInstance>("obi.png");
	transition = eps::CreateUnique<SpriteInstance>("black.png");

	LevelLoader loader{ selectIndex };

	field = std::make_unique<MapchipField>();

	fieldRotation = std::make_unique<WorldInstance>();

	crate_field_view();

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

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(RenderNodeConfig::NoClearRenderTarget);
	spriteNode->set_render_target_SC();

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({ bgSpriteNode,object3dNode,skinningMeshNode,outlineNode,spriteNode });


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

	backTitleSprite[0] = std::make_unique<SpriteInstance>("ESCkeyController.png");
	backTitleSprite[0]->get_transform().set_scale({ 0.5f, 1.0f });
	backTitleSprite[0]->get_uv_transform().set_scale({ 0.5f, 1.0f });
	backTitleSprite[0]->get_transform().set_translate({ 1141,30 });
	backTitleSprite[1] = std::make_unique<SpriteInstance>("ESCkey.png");
	backTitleSprite[1]->get_transform().set_scale({ 0.5f, 1.0f });
	backTitleSprite[1]->get_uv_transform().set_scale({ 0.5f, 1.0f });
	backTitleSprite[1]->get_transform().set_translate({ 1141,30 });

	//if (!toSelectBack) {
	fromGameBack = std::make_unique<SpriteInstance>("ResetBack.png", Vector2(0.5f, 0.5f));
	fromGameBack->get_transform().set_translate({ 640.0f, -360.0f });
	//}
	//if (!fromGameKoara) {
	fromGameKoara = std::make_unique<SpriteInstance>("KoaraFace.png", Vector2(0.5f, 0.5f));
	fromGameKoara->get_transform().set_translate({ 640.0f, -360.0f });
	//}

	// 入力遅延時間
	InputDowntime = 0.3f;
	inputTimer = InputDowntime;
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

	numberUi->get_uv_transform().set_translate_x(selectIndex * 0.1f);
	numberUi10->get_uv_transform().set_translate_x((selectIndex / 10) * 0.1f);

	Quaternion rotation = fieldRotation->get_transform().get_quaternion();
	fieldRotation->get_transform().set_quaternion(Quaternion::AngleAxis(CVector3::BASIS_Y, PI2 / 3 * WorldClock::DeltaSeconds()) * rotation);

	background->update();

	from_game_update();
}

void SelectScene::begin_rendering() {
	//	camera3D->update_matrix();
	fieldRotation->update_affine();
	field->begin_rendering();
	//goalMesh->begin_rendering();
	//parentKoala->begin_rendering();
	//childKoala->begin_rendering();

	numberUi->begin_rendering();
	numberUi10->begin_rendering();
	selectUi->begin_rendering();
	startUi[0]->begin_rendering();
	startUi[1]->begin_rendering();
	obSprite->begin_rendering();
	background->begin_rendering();
	transition->begin_rendering();
	backTitleSprite[0]->begin_rendering();
	backTitleSprite[1]->begin_rendering();
	fromGameBack->begin_rendering();
	fromGameKoara->begin_rendering();
}

void SelectScene::late_update() {
}

void SelectScene::draw() const {
	renderPath->begin();
	// 背景スプライト
	background->draw();
	renderPath->next();
	// Mesh
	camera3D->register_world_projection(1);
	camera3D->register_world_lighting(4);
	//directionalLight->register_world(5);
	//field->draw();
	//goalMesh->draw();

	renderPath->next();
	// SkinningMesh
	camera3D->register_world_projection(1);
	camera3D->register_world_lighting(5);
	//directionalLight->register_world(6);
	//parentKoala->draw();
	//childKoala->draw();
	//background->animeDraw();
	renderPath->next();
	outlineNode->draw();

	renderPath->next();
	// 前景スプライト
	obSprite->draw();
	numberUi->draw();
	numberUi10->draw();
	selectUi->draw();
	startUi[(int)GameValue::UiType.get_type()]->draw();
	backTitleSprite[(int)GameValue::UiType.get_type()]->draw();

	fromGameBack->draw();
	fromGameKoara->draw();

	transition->draw();

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
}

void SelectScene::default_update() {
	//フェード処理
	transitionTimer += WorldClock::DeltaSeconds();
	if (fadeEase >= 0) {
		fadeEase -= WorldClock::DeltaSeconds();
		float fadeRatio = std::max(0.f, fadeEase / 1.0f);
		transition->get_color().alpha = fadeEase;
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

		backTitleSprite[0]->get_uv_transform().set_translate_x(0.5f);
		backTitleSprite[1]->get_uv_transform().set_translate_x(0.5f);
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
	transition->get_color().alpha = fadeRatio;
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
}
#endif // _DEBUG
