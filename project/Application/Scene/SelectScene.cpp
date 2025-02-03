#include "SelectScene.h"

#include <Library/Math/Definition.h>

#include "Engine/Module/Render/RenderNode/2D/Sprite/SpriteNode.h"
#include "Engine/Resources/Audio/AudioManager.h"
#include <Engine/Module/Render/RenderNode/Forward/Object3DNode/Object3DNode.h>
#include <Engine/Module/World/Camera/Camera2D.h>
#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Resources/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Resources/Texture/TextureManager.h>
#include <Engine/Runtime/Input/Input.h>
#include <Engine/Runtime/Scene/SceneManager.h>
#include <Engine/Utility/Tools/SmartPointer.h>

#include "Application/GameValue.h"
#include "Application/LevelLoader/LevelLoader.h"
#include "Application/Scene/GameScene.h"

SelectScene::SelectScene() : SelectScene(1) {};

SelectScene::SelectScene(int32_t selectLevel) :
	selectIndex(selectLevel) {
}

SelectScene::~SelectScene() = default;

void SelectScene::load() {

	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/RordObj/RordObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/WallObj/WallObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/IceObj/IceObj.obj");

	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/StageSelectUI.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/start.png");
	TextureManager::RegisterLoadQue("./GameResources/Texture/UI/number.png");
	AudioManager::RegisterLoadQue("./GameResources/Audio/BGM/Title.wav");

}

void SelectScene::initialize() {
	// Instance
	Camera2D::Initialize();
	camera3D = std::make_unique<Camera3D>();
	camera3D->initialize();
	camera3D->set_transform({
		CVector3::BASIS,
		 Quaternion::EulerDegree(40,0,0),
		{0,10,-12}
		});
	directionalLight = eps::CreateUnique<DirectionalLightInstance>();

	startUi = eps::CreateUnique<SpriteInstance>("start.png", Vector2{ 0.5f, 0.5f });
	startUi->get_transform().set_translate({ 640.0f,140 });
	selectUi = eps::CreateUnique<SpriteInstance>("StageSelectUI.png", Vector2{ 0.5f, 0.5f });
	selectUi->get_transform().set_translate({ 640.0f,550.0f });
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

	LevelLoader loader{ selectIndex };

	field = std::make_unique<MapchipField>();
	fieldRotation = std::make_unique<WorldInstance>();

	crate_field_view();

	// Node&Path
	std::shared_ptr<Object3DNode> object3dNode;
	object3dNode = std::make_unique<Object3DNode>();
	object3dNode->initialize();
	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	std::shared_ptr<SpriteNode> spriteNode;
	spriteNode = std::make_unique<SpriteNode>();
	spriteNode->initialize();
	spriteNode->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueDrawBefore);
	spriteNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	renderPath = eps::CreateUnique<RenderPath>();
	renderPath->initialize({ object3dNode,spriteNode });

	bgm = std::make_unique<AudioPlayer>();
	bgm->initialize("Game.wav");
	bgm->set_loop(true);
	bgm->set_volume(0.1f);
	bgm->play();
}

void SelectScene::popped() {
}

void SelectScene::finalize() {
}

void SelectScene::begin() {
}

void SelectScene::update() {
	if (Input::IsTriggerKey(KeyID::D) && selectIndex < GameValue::MaxLevel) {
		++selectIndex;
		crate_field_view();
	}
	else if (Input::IsTriggerKey(KeyID::A) && selectIndex > 1) {
		--selectIndex;
		crate_field_view();
	}

	// 2桁表示
	if (selectIndex >= 10) {
		numberUi->get_transform().set_translate({ 640.0f+ 96 / 2,360.0f });
		numberUi10->set_active(true);
	}
	// 1桁表示
	else {
		numberUi->get_transform().set_translate({ 640.0f,360.0f });
		numberUi10->set_active(false);
	}

	numberUi->get_uv_transform().set_translate_x(selectIndex * 0.1f);
	numberUi10->get_uv_transform().set_translate_x((selectIndex / 10) * 0.1f);

	if (Input::IsTriggerKey(KeyID::Space)) {
		SceneManager::SetSceneChange(
			eps::CreateUnique<GameScene>(selectIndex), 1.0f
		);
	}

	Quaternion rotation = fieldRotation->get_transform().get_quaternion();
	fieldRotation->get_transform().set_quaternion(Quaternion::AngleAxis(CVector3::BASIS_Y, 1.5f * ToRadian) * rotation);
}

void SelectScene::begin_rendering() {
	camera3D->update_matrix();
	fieldRotation->update_affine();
	field->begin_rendering();

	numberUi->begin_rendering();
	numberUi10->begin_rendering();
	selectUi->begin_rendering();
	startUi->begin_rendering();
}

void SelectScene::late_update() {
}

void SelectScene::draw() const {
	renderPath->begin();
	// Mesh
	camera3D->register_world_projection(1);
	camera3D->register_world_lighting(4);
	directionalLight->register_world(5);
	field->draw();

	renderPath->next();
	// Sprite
	numberUi->draw();
	numberUi10->draw();
	selectUi->draw();
	startUi->draw();

	renderPath->next();
}

void SelectScene::crate_field_view() {
	fieldRotation->get_transform().set_quaternion(CQuaternion::IDENTITY);

	LevelLoader loader{ selectIndex };
	field->initialize(loader);
	Reference<WorldInstance> fieldRoot = field->field_root();


	fieldRoot->reparent(fieldRotation, false);
	fieldRoot->get_transform().set_translate(
		{ -static_cast<float>(field->row()) / 2,0,-static_cast<float>(field->column()) / 2 }
	);
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
}
#endif // _DEBUG
