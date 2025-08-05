#include "TitleScene.h"

#include <Engine/Assets/Animation/NodeAnimation/NodeAnimationLibrary.h>
#include <Engine/Assets/Animation/Skeleton/SkeletonLibrary.h>
#include <Engine/Assets/Audio/AudioLibrary.h>
#include <Engine/Assets/Audio/AudioManager.h>
#include <Engine/Assets/PolygonMesh/PolygonMeshLibrary.h>
#include <Engine/Assets/Shader/ShaderLibrary.h>
#include <Engine/Assets/Texture/TextureLibrary.h>
#include <Engine/GraphicsAPI/DirectX/DxSwapChain/DxSwapChain.h>
#include <Engine/Module/Render/RenderNode/2D/Sprite/SpriteNode.h>
#include <Engine/Module/Render/RenderNode/Forward/Mesh/SkinningMeshNodeForward.h>
#include <Engine/Module/Render/RenderNode/Forward/Mesh/StaticMeshNodeForward.h>
#include <Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h>
#include <Engine/Module/World/Camera/Camera2D.h>
#include <Engine/Module/World/Mesh/SkinningMeshInstance.h>
#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Runtime/Clock/WorldClock.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/Scene/SceneManager.h>

#include "Application/GameValue.h"
#include "Application/Scene/SelectScene.h"

#include "Application/PostEffect/BloomNode.h"
#include "Application/PostEffect/GaussianBlurNode.h"
#include "Application/PostEffect/LuminanceExtractionNode.h"
#include "Application/PostEffect/MargeTextureNode.h"

#include <Library/Utility/Tools/SmartPointer.h>

#include <utility>

#include "../Configuration/Configuration.h"

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::load() {
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/start.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/StartController.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/start_EN.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/StartController_EN.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/black.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/TitleLogo.png");
	TextureLibrary::RegisterLoadQue("./GameResources/Texture/UI/ChangeLanguage.png");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/BGM/TitleBGM.wav");

	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	PolygonMeshLibrary::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	SkeletonLibrary::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/ParentKoala/ParentKoala.gltf");
	NodeAnimationLibrary::RegisterLoadQue("./GameResources/Models/ChiledKoala/ChiledKoala.gltf");

	AudioLibrary::RegisterLoadQue("./GameResources/Audio/gameStart.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/move.wav");
	AudioLibrary::RegisterLoadQue("./GameResources/Audio/SelectFaild.wav");

	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/Forward/Mesh/StaticMeshForward.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/Forward/Mesh/SkinningMeshForward.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/Forward/Forward.PS.hlsl");
	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/Forward/ForwardAlpha.PS.hlsl");
	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/Posteffect/Outline/Outline.PS.hlsl");
	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/FullscreenShader.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/Sprite/Sprite.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/Sprite/Sprite.PS.hlsl");
	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/Forward/Particle/ParticleMesh/ParticleMesh.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/Forward/Particle/ParticleMesh/ParticleMesh.PS.hlsl");
	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/Misc/PrimitiveGeometry/PrimitiveGeometry.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("./EngineResources/HLSL/Misc/PrimitiveGeometry/PrimitiveGeometry.PS.hlsl");

	ShaderLibrary::RegisterLoadQue("./GameResources/HLSL/Bloom.PS.hlsl");
	ShaderLibrary::RegisterLoadQue("./GameResources/HLSL/GaussianBlur.PS.hlsl");
	ShaderLibrary::RegisterLoadQue("./GameResources/HLSL/LuminanceExtraction.PS.hlsl");
	ShaderLibrary::RegisterLoadQue("./GameResources/HLSL/MargeTexture4.PS.hlsl");
}

void TitleScene::initialize() {
	DxSwapChain::SetClearColor(CColor4::BLACK);

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
	languageSelection->get_material().uvTransform.set_scale({ 0.5f, 1.0f });

	renderTextures.resize(7);
	renderTextures[0].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB); // シーンアウト
	renderTextures[1].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB); // 輝度抽出
	renderTextures[2].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, EngineSettings::CLIENT_WIDTH / 2, EngineSettings::CLIENT_HEIGHT / 2); // ダウンサンプリング 1/2
	renderTextures[3].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, EngineSettings::CLIENT_WIDTH / 4, EngineSettings::CLIENT_HEIGHT / 4); // ダウンサンプリング 1/4
	renderTextures[4].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, EngineSettings::CLIENT_WIDTH / 8, EngineSettings::CLIENT_HEIGHT / 8); // ダウンサンプリング 1/8
	renderTextures[5].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, EngineSettings::CLIENT_WIDTH / 16, EngineSettings::CLIENT_HEIGHT / 16); // ダウンサンプリング 1/16
	renderTextures[6].initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB); // ダウンサンプリングを合成

	baseRenderTexture.initialize(renderTextures[0]);
	luminanceRenderTexture.initialize(renderTextures[1]);
	downSampleRenderTexture2.initialize(renderTextures[2]);
	downSampleRenderTexture4.initialize(renderTextures[3]);
	downSampleRenderTexture8.initialize(renderTextures[4]);
	downSampleRenderTexture16.initialize(renderTextures[5]);
	bloomBaseRenderTexture.initialize(renderTextures[6]);

	// Node&Path
	std::shared_ptr<StaticMeshNodeForward> object3dNode;
	object3dNode = std::make_unique<StaticMeshNodeForward>();
	object3dNode->initialize();
	object3dNode->set_render_target(baseRenderTexture);

	std::shared_ptr<SkinningMeshNodeForward> skinningMeshNode;
	skinningMeshNode = std::make_unique<SkinningMeshNodeForward>();
	skinningMeshNode->initialize();
	skinningMeshNode->set_config(RenderNodeConfig::NoClearDepth | RenderNodeConfig::NoClearRenderTarget);
	skinningMeshNode->set_render_target(baseRenderTexture);

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(RenderNodeConfig::NoClearRenderTarget);
	spriteNode->set_render_target(baseRenderTexture);

	luminanceExtractionNode = eps::CreateShared<LuminanceExtractionNode>();
	luminanceExtractionNode->initialize();
	luminanceExtractionNode->set_render_target(luminanceRenderTexture);
	luminanceExtractionNode->set_texture_resource(renderTextures[0]);

	gaussianBlurNode2 = eps::CreateShared<GaussianBlurNode>();
	gaussianBlurNode2->initialize();
	gaussianBlurNode2->set_render_target(downSampleRenderTexture2);
	gaussianBlurNode2->set_base_texture(renderTextures[1]);

	gaussianBlurNode4 = eps::CreateShared<GaussianBlurNode>();
	gaussianBlurNode4->initialize();
	gaussianBlurNode4->set_render_target(downSampleRenderTexture4);
	gaussianBlurNode4->set_base_texture(renderTextures[2]);

	gaussianBlurNode8 = eps::CreateShared<GaussianBlurNode>();
	gaussianBlurNode8->initialize();
	gaussianBlurNode8->set_render_target(downSampleRenderTexture8);
	gaussianBlurNode8->set_base_texture(renderTextures[3]);

	gaussianBlurNode16 = eps::CreateShared<GaussianBlurNode>();
	gaussianBlurNode16->initialize();
	gaussianBlurNode16->set_render_target(downSampleRenderTexture16);
	gaussianBlurNode16->set_base_texture(renderTextures[4]);

	margeTextureNode = eps::CreateShared<MargeTextureNode>();
	margeTextureNode->initialize();
	margeTextureNode->set_render_target(bloomBaseRenderTexture);
	margeTextureNode->set_texture_resources({ renderTextures[2] ,renderTextures[3] ,renderTextures[4], renderTextures[5] });

	bloomNode = eps::CreateShared<BloomNode>();
	bloomNode->initialize();
	bloomNode->set_render_target_SC();
	bloomNode->set_base_texture(renderTextures[0]);
	bloomNode->set_blur_texture(renderTextures[6]);

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({ object3dNode,skinningMeshNode,spriteNode,
		luminanceExtractionNode, gaussianBlurNode2, gaussianBlurNode4, gaussianBlurNode8, gaussianBlurNode16, margeTextureNode, bloomNode });

	// ---------------------- DrawManager ----------------------
	staticMeshDrawManager = std::make_unique<StaticMeshDrawManager>();
	skinningMeshDrawManager = std::make_unique<SkinningMeshDrawManager>();
	skinningMeshDrawManager->initialize(1);
	skinningMeshDrawManager->make_instancing(0, "ParentKoala.gltf", 1);
	skinningMeshDrawManager->make_instancing(0, "ChiledKoala.gltf", 1);

	spriteDrawExecutors.resize(2);
	spriteDrawExecutors[0] = std::make_unique<SpriteDrawExecutor>();
	spriteDrawExecutors[0]->reinitialize(1);
	spriteDrawExecutors[1] = std::make_unique<SpriteDrawExecutor>();
	spriteDrawExecutors[1]->reinitialize(100);

	directionalLightingExecutor = std::make_unique<DirectionalLightingExecutor>();
	directionalLightingExecutor->reinitialize(1);

	bgm = std::make_unique<AudioPlayer>();
	bgm->initialize("TitleBGM.wav");
	bgm->set_loop(true);
	bgm->set_volume(GameValue::TitleBgmVolume);
	bgm->play();

	parentObj = std::make_unique<SkinningMeshInstance>("ParentKoala.gltf", "Hello", true);
	parentObj->get_transform().set_translate({ 0.6f,-0.8f,3.7f });
	parentObj->get_transform().set_quaternion(Quaternion::EulerDegree(0.f, 0.f, 10.f));

	chiledObj = std::make_unique<SkinningMeshInstance>("ChiledKoala.gltf", "Hello", true);
	easeT = 0;
	movePos = { -7.f,-0.5f,14.4f };

	startAudio = std::make_unique<AudioPlayer>();
	startAudio->initialize("gameStart.wav");

	selectSeSuccussed = std::make_unique<AudioPlayer>();
	selectSeSuccussed->initialize("move.wav");
	selectSeFailed = std::make_unique<AudioPlayer>();
	selectSeFailed->initialize("SelectFaild.wav");
	selectSeFailed->set_volume(0.2f);

	skinningMeshDrawManager->register_instance(parentObj);
	skinningMeshDrawManager->register_instance(chiledObj);

	luminanceExtractionNode->set_param(0.67f, CColor3::WHITE);
	gaussianBlurNode2->set_parameters(1.0f, 30.48f, 8);
	gaussianBlurNode4->set_parameters(1.0f, 30.48f, 8);
	gaussianBlurNode8->set_parameters(1.0f, 30.48f, 8);
	gaussianBlurNode16->set_parameters(1.0f, 30.48f, 8);
	bloomNode->set_param(0.247f);
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
			languageSelection->get_material().uvTransform.set_translate({ 0.5f, 0.0f });
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
			languageSelection->get_material().uvTransform.set_translate({ 0.0f, 0.0f });
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
	parentObj->update_animation();
	chiledObj->update_animation();
}

void TitleScene::begin_rendering() {
	for(auto& executor : spriteDrawExecutors) {
		executor->begin();
	}
	directionalLightingExecutor->begin();

	spriteDrawExecutors[1]->write_to_buffer(startUi[(int)GameValue::UiType.get_type() + 2 * (size_t)Configuration::GetLanguage()]);
	spriteDrawExecutors[1]->write_to_buffer(titleLogo);
	if (transition->get_material().color.alpha > 0.0f) {
		spriteDrawExecutors[0]->write_to_buffer(transition);
	}
	spriteDrawExecutors[1]->write_to_buffer(languageSelection);

	camera3D->update_affine();
	directionalLight->update_affine();
	parentObj->update_affine();
	chiledObj->update_affine();

	camera3D->transfer();

	skinningMeshDrawManager->transfer();

	directionalLightingExecutor->write_to_buffer(directionalLight);
}

void TitleScene::late_update() {}

void TitleScene::draw() const {
	// Mesh
	renderPath->begin();

	// SkinningMesh
	renderPath->next();
	camera3D->register_world_projection(2);
	camera3D->register_world_lighting(5);
	directionalLightingExecutor->set_command(6);
	skinningMeshDrawManager->draw_layer(0);

	// Sprite
	renderPath->next();
	spriteDrawExecutors[1]->draw_command();
	spriteDrawExecutors[0]->draw_command();

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

void TitleScene::in_update() {
	transitionTimer += WorldClock::DeltaSeconds();
	float parametric = transitionTimer / 0.5f;
	transition->get_material().color.alpha = 1 - std::min(1.f, parametric);
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
	transition->get_material().color.alpha = parametric;
	bgm->set_volume((1 - parametric) * 0.2f);
}

#ifdef _DEBUG
void TitleScene::debug_update() {
	//ImGui::Begin("parent");
	//chiledObj->debug_gui();
	//ImGui::End();

	ImGui::Begin("Light");
	directionalLight->debug_gui();
	ImGui::End();

	ImGui::Begin("Camera");
	camera3D->debug_gui();
	ImGui::End();

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

