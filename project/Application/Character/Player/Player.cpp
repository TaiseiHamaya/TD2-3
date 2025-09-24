#include "Player.h"
#include "Engine/Runtime/Input/Input.h"
#include "Application/Utility/GameUtility.h"

#include "Engine/Rendering/DirectX/DirectXResourceObject/ConstantBuffer/Material/Material.h"
#include <Engine/Resources/Animation/NodeAnimation/NodeAnimationPlayer.h>
#include "State/PlayerStateIdle.h"
#include "State/PlayerStateRotateCheck.h"
#include "State/PlayerStateRotate.h"
#include "State/PlayerStateRotateFailed.h"
#include "State/PlayerStateMoveCheck.h"
#include "State/PlayerStateMove.h"
#include "State/PlayerStateMoveFailed.h"
#include "State/PlayerStateIceCheck.h"
#include <Engine/Module/DrawExecutor/Mesh/SkinningMeshDrawManager.h>

Player::Player() {
	object_ = std::make_unique<SkinningMeshInstance>();
	exclamation_ = std::make_unique<SkinningMeshInstance>();
	flusteredEffect_ = std::make_unique<SkinningMeshInstance>();

	moveAudio = std::make_unique<AudioPlayer>();
	unmovable = std::make_unique<AudioPlayer>();
	fall = std::make_unique<AudioPlayer>();
	iceMove = std::make_unique<AudioPlayer>();
	rotatAudio = std::make_unique<AudioPlayer>();
}

void Player::initialize(const LevelLoader& level, MapchipField* mapchipField) {
	// ビックリマークの生成
	exclamationData_.exclamation_ = std::make_unique<AnimatedMeshInstance>();
	exclamationData_.exclamation_->reset_animated_mesh("exclamation.gltf", "Standby", false);

	// ステートの設定
	states_["Idle"] = std::make_unique<PlayerStateIdle>();
	states_["RotateCheck"] = std::make_unique<PlayerStateRotateCheck>();
	states_["Rotate"] = std::make_unique<PlayerStateRotate>();
	states_["RotateFailed"] = std::make_unique<PlayerStateRotateFailed>();
	states_["MoveCheck"] = std::make_unique<PlayerStateMoveCheck>();
	states_["IceCheck"] = std::make_unique<PlayerStateIceCheck>();
	states_["Move"] = std::make_unique<PlayerStateMove>();
	states_["MoveFailed"] = std::make_unique<PlayerStateMoveFailed>();
	currentState_ = states_["Idle"].get();

	object_ = std::make_unique<AnimatedMeshInstance>();


	object_->reset_animated_mesh("ParentKoala.gltf", "Standby", true);
	object_->get_transform().set_translate(level.get_player_position());
	object_->set_active(true);
	object_->get_transform().set_scale(CVector3::ONE);
	object_->update_affine();

	playerState = PlayerState::Idle;

	// ビックリマークの生成
	exclamation_->reset_animated_mesh("exclamation.gltf", "Standby", false);
	exclamation_->set_active(false);

	auto& objMat = object_->get_materials();
	for (auto& mat : objMat) {
		mat.lightingType = LighingType::None;
	}
	mapchipField_ = mapchipField;

	flusteredEffect_->reset_animated_mesh("FlusteredEffect.gltf", "Standby", true);
	flusteredEffect_->get_animation()->set_time_force(1000);

	//音関連
	moveAudio->initialize("move.wav");
	unmovable->initialize("unmovable.wav");
	fall->initialize("fall.wav");
	iceMove->initialize("iceMove.wav");

	rotatAudio->initialize("rotate.wav");

	fallSoundFlag = false;
	unmovableFlag = false;
	isMoved = false;
}

void Player::setup(Reference<SkinningMeshDrawManager> drawManager) {
	drawManager->register_instance(object_);
	drawManager->register_instance(exclamation_);
	//drawManager->register_instance(flusteredEffect_);
}

void Player::finalize() {}

void Player::update() {
	isStackMovement = false;
	object_->begin();
	exclamationData_.exclamation_->begin();
	flusteredEffect_->begin();
	isMove = false;
	moveNumOnIce = 1;

	isOnChild = false;
	isTurnSuccess = true;

	if (currentState_) {
		currentState_->Update(*this);
	}
	
	if (currentState_) {
		for (const auto& [name, state] : states_) {
			if (state.get() == currentState_) {
				ImGui::Begin("PlayerState");
				ImGui::Text("Current State: %s", name.c_str());
				ImGui::End();
				break;
			}
		}
	}


	// 子供の座標の上にビックリマークを置いておく
	exclamationData_.exclamation_->get_transform().set_translate(object_->world_position());
	exclamationData_.exclamation_->update();
	// 焦る時のエフェクトをプレイヤーの上に置いておく
	Vector3 flusteredPos = object_->world_position();
	flusteredPos.y += 1.0f;
	flusteredEffect_->get_transform().set_translate(flusteredPos);
	if (playerAnimation == PlayerAnimation::Flustered) {
		flusteredEffect_->set_active(true);
	}
	else {
		flusteredEffect_->set_active(false);
	}
	flusteredEffect_->update();

	if (isMoving && !preIsMoving) {
		object_->reset_animation("Walk", true);
	}
	else if (!isMoving && preIsMoving) {
		if (is_out_ground()) {
			object_->reset_animation("Flustered", true);
		}
		else {
			object_->reset_animation("Standby", true);
		}
	}
	object_->update();
	// 一フレーム前の移動方向を保存しておく
	preMoveDirection = moveDirection;
	preIsMoving = isMoving;
}

void Player::begin_rendering() {
	object_->begin_rendering();
	exclamationData_.exclamation_->begin_rendering();
	flusteredEffect_->begin_rendering();
}

void Player::draw() const {
	object_->draw();
	if (exclamationData_.isActive) {
		exclamationData_.exclamation_->draw();
	}

void Player::update_affine() {
	object_->update_animation();
	object_->update_affine();
	exclamation_->update_animation();
	exclamation_->update_affine();
	flusteredEffect_->update_affine();
	flusteredEffect_->update_animation();
}

void Player::on_undo(Vector3 position, Quaternion rotation, bool setParent) {
	object_->get_transform().set_translate(position);
	object_->get_transform().set_quaternion(
		rotation
	);
	moveDirection = -CVector3::BASIS_Z * rotation;
	isParent = setParent;
}

#ifdef _DEBUG
void Player::debug_update() {
	//ImGui::Begin("Player");
	//ImGui::Text("%d", isParent);
	//object_->debug_gui();
	//ImGui::End();

	//ImGui::Begin("flust");
	//flusteredEffect_->debug_gui();
	//ImGui::End();
}
#endif // _DEBUG


void Player::fall_update() {
	if (playerState != PlayerState::Falling) {
		return;
	}

	if (!isFalling) {
		isFalled = false;
		playerState = PlayerState::Idle;
		return;
	}

	object_->begin();
	Vector3 position = object_->get_transform().get_translate();
	if (!fallSoundFlag) {
		fall->play();
		fallSoundFlag = true;
	}

	position.y -= fallSpeed * WorldClock::DeltaSeconds();
	object_->get_transform().set_translate(position);
	if (position.y <= -3.0f) {
		isFalled = true;
		object_->update();
	}
}

void Player::change_state(const std::string& stateName) {
	currentState_->Exit(*this);
	auto it = states_.find(stateName);
	if (it != states_.end()) {
		currentState_ = it->second.get();
		currentState_->Enter(*this);
	}
}
