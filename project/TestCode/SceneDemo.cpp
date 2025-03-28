#include "SceneDemo.h"

#include "CallbackManagerDemo.h"
#include "Engine/Module/World/AnimatedMesh/AnimatedMeshInstance.h"
#include "Engine/Module/World/Camera/Camera3D.h"
#include "Engine/Module/World/Collision/Collider/SphereCollider.h"
#include "Engine/Module/World/Collision/Collider/AABBCollider.h"
#include "Engine/Module/World/Collision/CollisionManager.h"
#include "Engine/Module/World/Mesh/MeshInstance.h"
#include "Engine/Resources/Animation/NodeAnimation/NodeAnimationManager.h"
#include "Engine/Resources/Animation/Skeleton/SkeletonManager.h"
#include "Engine/Resources/PolygonMesh/PolygonMeshManager.h"
#include "Engine/Runtime/Scene/SceneManager.h"
#include "Library/Math/Hierarchy.h"

#include "Engine/Module/World/Camera/Camera2D.h"
#include "Engine/Module/World/Sprite/SpriteInstance.h"

#include "Library/Math/Color4.h"

#include "Engine/Debug/DebugValues/DebugValues.h"
#include "Engine/Module/Render/RenderPath/RenderPath.h"
#include "Engine/Rendering/DirectX/DirectXSwapChain/DirectXSwapChain.h"
#include "Engine/Resources/Audio/AudioManager.h"
#include "Engine/Resources/Texture/TextureManager.h"

#include "Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h"
#include "Engine/Module/Render/RenderTargetGroup/MultiRenderTarget.h"
#include "Engine/Rendering/DirectX/DirectXResourceObject/DepthStencil/DepthStencil.h"
#include "Engine/Utility/Template/Behavior.h"
#include "Engine/Utility/Tools/SmartPointer.h"

#include "Engine/Module/Render/RenderNode/Deferred/Mesh/MeshNodeDeferred.h"
#include "Engine/Module/Render/RenderNode/Deferred/SkinMesh/SkinMeshNodeDeferred.h"
#include "Engine/Module/Render/RenderNode/Deferred/Lighting/DirectionalLighingNode.h"
#include "Engine/Module/Render/RenderNode/Debug/PrimitiveLine/PrimitiveLineNode.h"
#include "Engine/Debug/ImGui/ImGuiLoadManager/ImGuiLoadManager.h"

#include "Engine/Debug/Output.h"

//#define QUATERNION_SERIALIZER
#define TRANSFORM3D_SERIALIZER
#define TRANSFORM2D_SERIALIZER
#define VECTOR3_SERIALIZER
#define VECTOR2_SERIALIZER
#define COLOR3_SERIALIZER
#define COLOR4_SERIALIZER
#include <Engine/Resources/Json/JsonSerializer.h>

SceneDemo::SceneDemo() = default;

SceneDemo::~SceneDemo() = default;

void SceneDemo::load() {
	PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/Primitive/Sphere.obj");
	PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/Bone/bone.obj");
	PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/gltf-test/Boss_RangedAttack.gltf");
	AudioManager::RegisterLoadQue("./EngineResources/Alarm01.wav");
	AudioManager::RegisterLoadQue("./EngineResources/Texture/CircularGaugeTexter.png");
	// 存在しないファイルをロードしようとするとエラー出力が出る
	AudioManager::RegisterLoadQue("./Engine/Resources/SE_meteoEachOther.wav");
	PolygonMeshManager::RegisterLoadQue("./Engine/Resources/SE_meteoEachOther.wav");
	TextureManager::RegisterLoadQue("./Engine/Resources/SE_meteoEachOther.wav");

	PolygonMeshManager::RegisterLoadQue("./EngineResources/Models/Player.gltf");
	NodeAnimationManager::RegisterLoadQue("./EngineResources/Models/Player.gltf");
	SkeletonManager::RegisterLoadQue("./EngineResources/Models/Player.gltf");

//gameObjectをロード
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/RordObj/RordObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/WallObj/WallObj.obj");
	PolygonMeshManager::RegisterLoadQue("./GameResources/Models/GoalObj/GoalObj.obj");

}

void SceneDemo::initialize() {
	Camera2D::Initialize();
	camera3D = std::make_unique<Camera3D>();
	camera3D->initialize();
	//camera3D->set_transform({
	//	CVector3::BASIS,
	//	Quaternion::EulerDegree(45,0,0),
	//	{0,10,-10}
	//	});
	//camera3D->from_json();

	{
		//animationPlayer.isLoop = true;
		//animationPlayer.timer = 0;
		//animationPlayer.animationName = "animation_AnimatedCube";
		//animationPlayer.nodeName = "AnimatedCube";
		//animationPlayer.nodeAnimation = nodeAnimationResource;
	}

	//testValue = jsonResource.try_emplace<WorldInstance>("name");
	jsonResource.register_value(__JSON_RESOURCE_REGISTER(testValue));

	parent = std::make_unique<MeshInstance>();
	parent->reset_mesh("Player.gltf");
	child = std::make_unique<MeshInstance>();
	child->reset_mesh("Sphere.obj");
	child->reparent(*parent);
	fieldObjs = std::make_unique<MapchipField>();
	//fieldObjs->init();
	debugCheckObj = std::make_unique<MeshInstance>();
	debugCheckObj->reset_mesh("ParentObj.obj");

	animatedMeshInstance = eps::CreateUnique<AnimatedMeshInstance>("Player.gltf", "Idle", true);

	parentCollider = std::make_unique<SphereCollider>(1.0f);
	parentCollider->reparent(*parent);

	childCollider = std::make_unique<SphereCollider>(1.0f);
	childCollider->reparent(*child);

	singleCollider = std::make_unique<SphereCollider>(1.0f);

	single2Collider = std::make_unique<AABBCollider>(Vector3{ 3.0f, 2.0f, 1.5f });
	single2Collider->get_transform().set_translate_x(-3.0f);

	single3Collider = std::make_unique<AABBCollider>(CVector3::BASIS, Vector3{ 0.3f,0.3f,0.3f });
	single3Collider->get_transform().set_translate_x(3.0f);

	particleEmitter = eps::CreateUnique<ParticleEmitterInstance>("test.json", 128);

	sprite = std::make_unique<SpriteInstance>("uvChecker.png");

	directionalLight = eps::CreateUnique<DirectionalLightInstance>();

	collisionManager = std::make_unique<CollisionManager>();
	collisionManager->set_callback_manager(
		eps::CreateUnique<CallbackManagerDemo>()
	);
	collisionManager->register_collider("Parent", parentCollider);
	collisionManager->register_collider("Single", singleCollider);
	collisionManager->register_collider("Single", single2Collider);
	collisionManager->register_collider("Single", single3Collider);
	collisionManager->register_collider("Child", childCollider);

	audioPlayer = std::make_unique<AudioPlayer>();
	audioPlayer->initialize("Alarm01.wav");

	///
	/// ここはForward
	/// 
//		// ---------------------- RT設定 ----------------------
//	std::shared_ptr<SingleRenderTarget> renderTarget = eps::CreateShared<SingleRenderTarget>();
//	renderTarget->initialize();
//
//	// ---------------------- StaticMesh ----------------------
//	std::shared_ptr<Object3DNode> object3dNode;
//	object3dNode = std::make_unique<Object3DNode>();
//	object3dNode->initialize();
//	object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
//	//object3dNode->set_render_target(renderTarget);
//	object3dNode->set_config(RenderNodeConfig::ContinueDrawBefore | RenderNodeConfig::ContinueUseDpehtBefore);
//	//object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
//
//	/// ---------------------- SkinMesh ----------------------
//	std::shared_ptr<SkinningMeshNode> skinningMeshNode;
//	skinningMeshNode = std::make_unique<SkinningMeshNode>();
//	skinningMeshNode->initialize();
//	skinningMeshNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
//	//skinningMeshNode->set_render_target(renderTarget);
//	skinningMeshNode->set_config(
//		RenderNodeConfig::ContinueDrawBefore |
//		RenderNodeConfig::ContinueUseDpehtBefore |
//		RenderNodeConfig::ContinueDrawAfter |
//		RenderNodeConfig::ContinueUseDpehtAfter
//	);
//	//object3dNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
//
//	// ---------------------- ParticleBillboard ----------------------
//	std::shared_ptr<ParticleMeshNode> particleBillboardNode;
//	particleBillboardNode = std::make_unique<ParticleMeshNode>();
//	particleBillboardNode->initialize();
//	particleBillboardNode->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueUseDpehtAfter);
//	//particleBillboardNode->set_render_target(renderTarget);
//	particleBillboardNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
//
//	// ---------------------- Sprite ----------------------
//	std::shared_ptr<SpriteNode> spriteNode;
//	spriteNode = std::make_unique<SpriteNode>();
//	spriteNode->initialize();
//	spriteNode->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueDrawBefore);
//	spriteNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
//
//#ifdef _DEBUG
//	// ---------------------- 線 ----------------------
//	std::shared_ptr<PrimitiveLineNode> primitiveLineNode;
//	primitiveLineNode = std::make_unique<PrimitiveLineNode>();
//	primitiveLineNode->initialize();
//#endif // _DEBUG
//
//	// ---------------------- 生成 ----------------------
//	renderPath = eps::CreateUnique<RenderPath>();
//#ifdef _DEBUG
//	renderPath->initialize({ object3dNode,skinningMeshNode,particleBillboardNode, spriteNode,primitiveLineNode });
//#else
//	renderPath->initialize({ object3dNode,skinningMeshNode,particleBillboardNode, spriteNode });
//#endif // _DEBUG

	///
	/// Deferredテスト用
	/// 
	auto deferredRenderTarget = DeferredAdaptor::CreateGBufferRenderTarget();

	auto deferredMeshNode = eps::CreateShared<MeshNodeDeferred>();
	deferredMeshNode->initialize();
	deferredMeshNode->set_render_target(deferredRenderTarget);
	deferredMeshNode->set_config(RenderNodeConfig::ContinueDrawBefore | RenderNodeConfig::ContinueUseDpehtBefore);

	auto skinMeshNodeDeferred = eps::CreateShared<SkinMeshNodeDeferred>();
	skinMeshNodeDeferred->initialize();
	skinMeshNodeDeferred->set_render_target(deferredRenderTarget);
	skinMeshNodeDeferred->set_config(RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueUseDpehtAfter);

	auto directionalLightingNode = eps::CreateShared<DirectionalLightingNode>();
	directionalLightingNode->initialize();
	directionalLightingNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
	directionalLightingNode->set_gbuffers(deferredRenderTarget);

#ifdef _DEBUG
	std::shared_ptr<PrimitiveLineNode> primitiveLineNode;
	primitiveLineNode = std::make_unique<PrimitiveLineNode>();
	primitiveLineNode->initialize();
#endif // _DEBUG

	renderPath = eps::CreateUnique<RenderPath>();
#ifdef _DEBUG
	renderPath->initialize({ deferredMeshNode,skinMeshNodeDeferred ,directionalLightingNode,primitiveLineNode });
#else
	renderPath->initialize({ deferredMeshNode,skinMeshNodeDeferred,directionalLightingNode });
#endif // _DEBUG

	//DirectXSwapChain::GetRenderTarget()->set_depth_stencil(nullptr);
	//DirectXSwapChain::SetClearColor(Color4{ 0.0f,0.0f,0.0f,0.0f });

	Particle::lookAtDefault = camera3D.get();
}

void SceneDemo::popped() {
}

void SceneDemo::finalize() {
}

void SceneDemo::begin() {
	collisionManager->begin();
	animatedMeshInstance->begin();
}

void SceneDemo::update() {
	//animationPlayer.update();
	//parent->get_transform().set_scale(
	//	animationPlayer.calculate_scale()
	//);
	//parent->get_transform().set_quaternion(
	//	animationPlayer.calculate_rotate()
	//);
	//parent->get_transform().set_translate(
	//	animationPlayer.calculate_translate()
	//);

	animatedMeshInstance->update();


	particleEmitter->update();
	directionalLight->update();
	fieldObjs->update();
}

void SceneDemo::begin_rendering() {
	camera3D->update_matrix();
	parent->begin_rendering();
	child->look_at(*camera3D);
	child->begin_rendering();
	sprite->begin_rendering();
	particleEmitter->begin_rendering();
	directionalLight->begin_rendering();

	animatedMeshInstance->begin_rendering();

	fieldObjs->begin_rendering();
	debugCheckObj->begin_rendering();
}

void SceneDemo::late_update() {
	collisionManager->update();
	collisionManager->collision("Parent", "Single");
	collisionManager->collision("Single", "Child");
	collisionManager->collision("Single", "Single");
}

void SceneDemo::draw() const {
	///
	/// Forward
	/// 
//	renderPath->begin();
//	camera3D->register_world_projection(1);
//	camera3D->register_world_lighting(4);
//	directionalLight->register_world(5);
//	parent->draw();
//	child->draw();
//#ifdef _DEBUG
//	camera3D->debug_draw_axis();
//	animatedMeshInstance->draw_skeleton();
//	DebugValues::ShowGrid();
//#endif // _DEBUG
//
//	renderPath->next();
//	camera3D->register_world_projection(1);
//	camera3D->register_world_lighting(5);
//	directionalLight->register_world(6);
//
//	animatedMeshInstance->draw();
//
//	renderPath->next();
//	camera3D->register_world_projection(1);
//	particleEmitter->draw();
//
//	renderPath->next();
//	sprite->draw();
//
//	renderPath->next();
//
//#ifdef _DEBUG
//	camera3D->register_world_projection(1);
//	collisionManager->debug_draw3d();
//	camera3D->debug_draw_frustum();
//
//	renderPath->next();
//#endif // _DEBUG

	///
	/// Deferred
	/// 
	renderPath->begin();
	camera3D->register_world_projection(1);
	parent->draw();
	child->draw();
#ifdef _DEBUG
	camera3D->debug_draw_axis();
	DebugValues::ShowGrid();
#endif // _DEBUG

	renderPath->next();
	camera3D->register_world_projection(1);
	animatedMeshInstance->draw();

	renderPath->next();
	directionalLight->register_world(0);
	camera3D->register_world_lighting(1);
	directionalLight->draw_deferred();

	renderPath->next();

#ifdef _DEBUG
	camera3D->register_world_projection(1);
	collisionManager->debug_draw3d();
	camera3D->debug_draw_frustum();

	renderPath->next();
#endif // _DEBUG

}

void SceneDemo::on_collision([[maybe_unused]] const BaseCollider* const other, Color4* object) {
	*object = { 1.0f,0,0,1.0f };
}

void SceneDemo::on_collision_enter(const BaseCollider* const other, Color4* object) {
	*object = { 0,1.0f,0,1.0f };
}

void SceneDemo::on_collision_exit(const BaseCollider* const, Color4* object) {
	*object = { 1.0f,1.0f,1.0f,1.0f };
}

#ifdef _DEBUG

#include <imgui.h>
void SceneDemo::debug_update() {
	ImGui::Begin("DemoScene");
	if (ImGui::Button("StackScene")) {
		SceneManager::SetSceneChange(std::make_unique<SceneDemo>(), 1, true);
	}
	if (ImGui::Button("ChangeScene")) {
		SceneManager::SetSceneChange(std::make_unique<SceneDemo>(), 1, false);
	}
	if (ImGui::Button("PopScene")) {
		SceneManager::PopScene(1);
	}
	ImGui::End();

	ImGui::Begin("Camera3D");
	camera3D->debug_gui();
	ImGui::End();

	ImGui::Begin("AnimatedMesh");
	animatedMeshInstance->debug_gui();
	if (ImGui::Button("Restart")) {
		animatedMeshInstance->get_animation()->restart();
	}
	ImGui::End();

	ImGui::Begin("Parent");
	parent->debug_gui();
	ImGui::End();

	ImGui::Begin("Child");
	child->debug_gui();
	ImGui::End();

	ImGui::Begin("Sprite");
	sprite->debug_gui();
	ImGui::End();

	ImGui::Begin("debugCheck");
	debugCheckObj->debug_gui();
	ImGui::End();

	AudioManager::DebugGui();

	ImGui::Begin("WorldClock");
	WorldClock::DebugGui();
	ImGui::End();

	ImGui::Begin("CollisionManager");
	collisionManager->debug_gui();
	ImGui::End();

	ImGui::Begin("DirectionalLight");
	directionalLight->debug_gui();
	ImGui::End();

	ImGuiLoadManager::ShowGUI();
}
#endif // _DEBUG
