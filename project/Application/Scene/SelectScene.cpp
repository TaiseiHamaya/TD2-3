#include "SelectScene.h"

#include <Library/Math/Definition.h>

#include <Engine/Module/Render/RenderNode/2D/Sprite/SpriteNode.h>
#include <Engine/Module/Render/RenderNode/Forward/Object3DNode/Object3DNode.h>
#include <Engine/Module/Render/RenderNode/Forward/SkinningMesh/SkinningMeshNode.h>
#include <Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h>
#include <Engine/Module/World/AnimatedMesh/AnimatedMeshInstance.h>
#include <Engine/Module/World/Camera/Camera2D.h>
#include <Engine/Module/World/Mesh/MeshInstance.h> 
#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Rendering/DirectX/DirectXResourceObject/DepthStencil/DepthStencil.h>
#include <Engine/Rendering/DirectX/DirectXResourceObject/OffscreenRender/OffscreenRender.h>
#include <Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Resources/Animation/NodeAnimation/NodeAnimationManager.h>
#include <Engine/Resources/Animation/Skeleton/SkeletonManager.h>
#include <Engine/Resources/Audio/AudioManager.h>
#include <Engine/Resources/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Resources/Texture/TextureManager.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Engine/Utility/Tools/SmartPointer.h>

#include "Application/Scene/TitleScene.h"

#include "Application/GameValue.h"
#include "Application/LevelLoader/LevelLoader.h"
#include "Application/Scene/GameScene.h"

SelectScene::SelectScene() : SelectScene(1) {};

SelectScene::SelectScene(int32_t selectLevel) :
	selectIndex(selectLevel) {
}

SelectScene::~SelectScene() = default;

void SelectScene::load() {
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/RordObj/RordObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/WallObj/WallObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/IceObj/IceObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObjStatic.obj");

	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/StageSelectUI.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/start.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/number.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/obi.png");

	AudioManager::RegisterLoadQue("./GameResources/Audio/BGM/Title.wav");
	TextureManager::RegisterLoadQue("./GameResources/Texture/backGround.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/backGround2.png");
	AudioManager::RegisterLoadQue("./GameResources/Audio/BGM/SelectBGM.wav");

	AudioManager::RegisterLoadQue("./GameResources/Audio/change.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/stageStart.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/backAudio.wav");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ESCkey.png");

	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.gltf");

}

void SelectScene::initialize() {
	sceneState = TransitionState::Main;

	// Instance
	Camera2D::Initialize();
	camera3D = std::make_unique<Camera3D>();
	camera3D->initialize();

	directionalLight = eps::CreateUnique<DirectionalLightInstance>();

	goalMesh = eps::CreateUnique<MeshInstance>("GoalObjStatic.obj");
	parentKoala = eps::CreateUnique<AnimatedMeshInstance>("ParentKoala.gltf", "Standby", true);
	childKoala = eps::CreateUnique<AnimatedMeshInstance>("ChiledKoala.gltf", "Standby", true);

	startUi = eps::CreateUnique<SpriteInstance>("start.png", Vector2{ 0.5f, 0.5f });
	startUi->get_transform().set_translate({ 640.0f,90 });
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

	std::shared_ptr<SingleRenderTarget> meshRT;
	meshRT = std::make_shared<SingleRenderTarget>();
	meshRT->initialize();

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
	outlineNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
	outlineNode->set_config(RenderNodeConfig::ContinueDrawBefore);
	outlineNode->set_depth_resource(DepthStencilValue::depthStencil->texture_gpu_handle());
	outlineNode->set_texture_resource(meshRT->offscreen_render().texture_gpu_handle());

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(
		RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueDrawBefore
	);
	spriteNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

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
	backTitle->set_volume(0.5f);

	backTitleSprite = std::make_unique<SpriteInstance>("ESCkey.png");
	backTitleSprite->get_transform().set_scale({ 0.5f, 1.0f });
	backTitleSprite->get_uv_transform().set_scale({ 0.5f, 1.0f });
	backTitleSprite->get_transform().set_translate({ 1141,30 });

	// 入力遅延時間
	InputDowntime = 0.3f;
}

void SelectScene::popped() {
}

void SelectScene::finalize() {
}

void SelectScene::begin() {
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
}

void SelectScene::begin_rendering() {
	camera3D->update_matrix();
	fieldRotation->update_affine();
	field->begin_rendering();
	goalMesh->begin_rendering();
	parentKoala->begin_rendering();
	childKoala->begin_rendering();

	numberUi->begin_rendering();
	numberUi10->begin_rendering();
	selectUi->begin_rendering();
	startUi->begin_rendering();
	obSprite->begin_rendering();
	background->begin_rendering();
	transition->begin_rendering();
	backTitleSprite->begin_rendering();
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
	directionalLight->register_world(5);
	field->draw();
	goalMesh->draw();

	renderPath->next();
	// SkinningMesh
	camera3D->register_world_projection(1);
	camera3D->register_world_lighting(5);
	directionalLight->register_world(6);
	parentKoala->draw();
	childKoala->draw();
	//background->animeDraw();
	renderPath->next();
	outlineNode->draw();

	renderPath->next();
	// 前景スプライト
	obSprite->draw();
	numberUi->draw();
	numberUi10->draw();
	selectUi->draw();
	startUi->draw();
	backTitleSprite->draw();
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

	Vector2 stickL = Input::StickL().normalize_safe(1e-4f, CVector2::ZERO);
	if (inputTimer <= 0.0f) {
		if (Input::IsPressKey(KeyID::D) || Input::IsPressKey(KeyID::Right) ||
			Input::IsPressPad(PadID::Right) || stickL.x > 0) {
			++selectIndex;
			selectIndex = (selectIndex - 1) % GameValue::MaxLevel + 1;
			crate_field_view();
			selectAudio->restart();
			inputTimer = InputDowntime;
		}
		else if (Input::IsPressKey(KeyID::A) || Input::IsPressKey(KeyID::Left) ||
			Input::IsPressPad(PadID::Left) || stickL.x < 0) {
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
	if (Input::IsTriggerKey(KeyID::Escape) || Input::IsTriggerPad(PadID::Back)) {
		SceneManager::SetSceneChange(
			eps::CreateUnique<TitleScene>(), 0.5f);
		backTitle->play();

		backTitleSprite->get_uv_transform().set_translate_x(0.5f);
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

#ifdef _DEBUG
void SelectScene::debug_update() {
	ImGui::Begin("SelectUi");
	selectUi->debug_gui();
	ImGui::End();
	ImGui::Begin("startUi");
	startUi->debug_gui();
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
