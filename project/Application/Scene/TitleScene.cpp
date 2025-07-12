#include "TitleScene.h"

#include "Engine/Module/Render/RenderNode/2D/Sprite/SpriteNode.h"
#include "Engine/Resources/Audio/AudioManager.h"
#include <Engine/Module/Render/RenderNode/Forward/Object3DNode/Object3DNode.h>
#include <Engine/Module/Render/RenderNode/Forward/SkinningMesh/SkinningMeshNode.h>
#include <Engine/Module/World/Camera/Camera2D.h>
#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Resources/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Resources/Texture/TextureManager.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Engine/Utility/Tools/SmartPointer.h>

#include "Application/GameValue.h"
#include "Application/Scene/SelectScene.h"

#include <Engine/Module/World/AnimatedMesh/AnimatedMeshInstance.h>
#include <Engine/Resources/Animation/NodeAnimation/NodeAnimationManager.h>
#include <Engine/Resources/Animation/Skeleton/SkeletonManager.h>
#include <utility>

#include "../Configuration/Configuration.h"

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::load() {
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/start.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/StartController.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/start_EN.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/StartController_EN.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/black.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/TitleLogo.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/ChangeLanguage.png");
	AudioManager::RegisterLoadQue("./GameResources/Audio/BGM/TitleBGM.wav");

	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	SkeletonManager::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	NodeAnimationManager::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");

	AudioManager::RegisterLoadQue("./GameResources/Audio/gameStart.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/move.wav");
	AudioManager::RegisterLoadQue("./GameResources/Audio/SelectFaild.wav");
}

void TitleScene::initialize() {
	Camera2D::Initialize();

	camera3D = std::make_unique<Camera3D>();
	camera3D->initialize();

	directionalLight = eps::CreateUnique<DirectionalLightInstance>();

	startUi[0] = eps::CreateUnique<SpriteInstance>("StartController.png", Vector2{ 0.5f, 0.5f });
	startUi[1] = eps::CreateUnique<SpriteInstance>("start.png", Vector2{ 0.5f, 0.5f });
	startUi[2] = eps::CreateUnique<SpriteInstance>("StartController_EN.png", Vector2{ 0.5f, 0.5f });
	startUi[3] = eps::CreateUnique<SpriteInstance>("start_EN.png", Vector2{ 0.5f, 0.5f });
	startUi[0]->get_transform().set_translate({ 440.0f,140 });
	startUi[1]->get_transform().set_translate({ 440.0f,140 });
	startUi[2]->get_transform().set_translate({ 440.0f,140 });
	startUi[3]->get_transform().set_translate({ 440.0f,140 });
	titleLogo = eps::CreateUnique<SpriteInstance>("TitleLogo.png", Vector2{ 0.5f, 0.5f });
	titleLogo->get_transform().set_translate({ 440.0f,540 });

	transition = eps::CreateUnique<SpriteInstance>("black.png", Vector2{ 0.f, 0.f });

	languageSelection = eps::CreateUnique<SpriteInstance>("ChangeLanguage.png", Vector2{ 0.0f, 0.0f });
	languageSelection->get_transform().set_translate({ 30.0f, 30.0f });
	languageSelection->get_transform().set_scale({ 0.5f, 1.0f });
	languageSelection->get_uv_transform().set_scale({ 0.5f, 1.0f });

	// Node&Path
	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_config(RenderNodeConfig::ContinueDrawBefore | RenderNodeConfig::ContinueUseDpehtBefore);
	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	std::shared_ptr<SkinningMeshNode> skinningMeshNode;
	skinningMeshNode = std::make_unique<SkinningMeshNode>();
	skinningMeshNode->initialize();
	skinningMeshNode->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueDrawBefore | RenderNodeConfig::ContinueUseDpehtAfter);
	skinningMeshNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(RenderNodeConfig::ContinueDrawAfter);
	spriteNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({ object3dNode,skinningMeshNode,spriteNode });

	bgm = std::make_unique<AudioPlayer>();
	bgm->initialize("TitleBGM.wav");
	bgm->set_loop(true);
	bgm->set_volume(GameValue::TitleBgmVolume);
	bgm->play();

	parentObj = std::make_unique<AnimatedMeshInstance>("ParentKoala.gltf", "Hello", true);
	parentObj->get_transform().set_translate({ 0.6f,-0.8f,3.7f });
	parentObj->get_transform().set_quaternion(Quaternion::EulerDegree(0.f, 0.f, 10.f));

	chiledObj = std::make_unique<AnimatedMeshInstance>("ChiledKoala.gltf", "Hello", true);
	easeT = 0;
	movePos = { -7.f,-0.5f,14.4f };

	startAudio = std::make_unique<AudioPlayer>();
	startAudio->initialize("gameStart.wav");

	selectSeSuccussed = std::make_unique<AudioPlayer>();
	selectSeSuccussed->initialize("move.wav");
	selectSeFailed = std::make_unique<AudioPlayer>();
	selectSeFailed->initialize("SelectFaild.wav");
	selectSeFailed->set_volume(0.2f);
}

void TitleScene::popped() {}

void TitleScene::finalize() {}

void TitleScene::begin() {
	if (Input::IsTriggerKey(KeyID::One)) {
		float masterVolume = AudioManager::GetMasterVolume();
		AudioManager::SetMasterVolume(masterVolume - 0.1f);
	}
	else if (Input::IsTriggerKey(KeyID::Two)) {
		float masterVolume = AudioManager::GetMasterVolume();
		AudioManager::SetMasterVolume(masterVolume + 0.1f);
	}
	if (Input::IsTriggerKey(KeyID::Three)) {
		GameValue::TitleBgmVolume -= 0.1f;
		bgm->set_volume(GameValue::TitleBgmVolume);
	}
	else if (Input::IsTriggerKey(KeyID::Four)) {
		GameValue::TitleBgmVolume += 0.1f;
		bgm->set_volume(GameValue::TitleBgmVolume);
	}
	GameValue::UiType.update();
	languageSelectTimer -= WorldClock::DeltaSeconds();
}

void TitleScene::update() {
	switch (sceneState) {
	case TransitionState::In:
		in_update();
		break;
	case TransitionState::Main:
		default_update();
		break;
	case TransitionState::Out:
		out_update();
		break;
	}

	float stickLx = Input::StickL().x;
	if (languageSelectTimer <= 0.0f) {
		Configuration::Language language = Configuration::GetLanguage();
		if (Input::IsPressKey(KeyID::Right) || Input::IsPressPad(PadID::Right) || stickLx >= 0.5f) {
			if (language == Configuration::Language::Japanese) {
				// SE成功
				selectSeSuccussed->restart();
			}
			else {
				// SE失敗
				selectSeFailed->restart();

			}
			Configuration::SetLanguage(Configuration::Language::English);
			languageSelection->get_uv_transform().set_translate({ 0.5f, 0.0f });
			languageSelectTimer = 0.7f;
		}
		else if (Input::IsPressKey(KeyID::Left) || Input::IsPressPad(PadID::Left) || stickLx <= -0.5f) {
			if (language == Configuration::Language::English) {
				// SE成功
				selectSeSuccussed->restart();
			}
			else {
				// SE失敗
				selectSeFailed->restart();
			}
			Configuration::SetLanguage(Configuration::Language::Japanese);
			languageSelection->get_uv_transform().set_translate({ 0.0f, 0.0f });
			languageSelectTimer = 0.7f;
		}
	}
	else {
		if (!(Input::IsPressKey(KeyID::Left) || Input::IsPressPad(PadID::Left) ||
			Input::IsPressKey(KeyID::Right) || Input::IsPressPad(PadID::Right) ||
			(stickLx >= 0.5f || stickLx <= -0.5f))
		   ) {
			languageSelectTimer = 0.0f;
		}
	}

	parentObj->begin();
	parentObj->update();
	chiledObj->begin();

	easeT += WorldClock::DeltaSeconds();

	movePos.x = std::lerp(-7.f, 7.f, easeT / totalEaseT);
	if (easeT > totalEaseT) { easeT = 0.f; }
	chiledObj->get_transform().set_translate(movePos);
	chiledObj->update();
}

void TitleScene::begin_rendering() {
	startUi[0]->begin_rendering();
	startUi[1]->begin_rendering();
	startUi[2]->begin_rendering();
	startUi[3]->begin_rendering();
	titleLogo->begin_rendering();
	transition->begin_rendering();
	languageSelection->begin_rendering();

	camera3D->update_matrix();
	directionalLight->begin_rendering();
	parentObj->begin_rendering();
	chiledObj->begin_rendering();
}

void TitleScene::late_update() {}

void TitleScene::draw() const {
	// Mesh
	renderPath->begin();
	camera3D->register_world_projection(1);
	camera3D->register_world_lighting(4);
	directionalLight->register_world(5);

	// SkinningMesh
	renderPath->next();
	camera3D->register_world_projection(1);
	camera3D->register_world_lighting(5);
	directionalLight->register_world(6);
	parentObj->draw();
	chiledObj->draw();
	// Sprite
	renderPath->next();
	startUi[(int)GameValue::UiType.get_type() + 2 * (size_t)Configuration::GetLanguage()]->draw();
	languageSelection->draw();
	titleLogo->draw();
	transition->draw();

	renderPath->next();
}

void TitleScene::in_update() {
	transitionTimer += WorldClock::DeltaSeconds();
	float parametric = transitionTimer / 0.5f;
	transition->get_color().alpha = 1 - std::min(1.f, parametric);
	if (parametric >= 1.0f) {
		sceneState = TransitionState::Main;
	}
}

void TitleScene::default_update() {
	if (Input::IsTriggerKey(KeyID::Space) || Input::IsTriggerPad(PadID::A)) {
		SceneManager::SetSceneChange(
			eps::CreateUnique<SelectScene>(), 1.0f);
		transitionTimer = WorldClock::DeltaSeconds();
		sceneState = TransitionState::Out;
		startAudio->play();
	}
}

void TitleScene::out_update() {
	transitionTimer += WorldClock::DeltaSeconds();
	float parametric = transitionTimer / 0.5f;
	transition->get_color().alpha = parametric;
	bgm->set_volume((1 - parametric) * 0.2f);
}

#ifdef _DEBUG
void TitleScene::debug_update() {
	ImGui::Begin("parent");
	chiledObj->debug_gui();
	ImGui::End();
}
#endif // _DEBUG

