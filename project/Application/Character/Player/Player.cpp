#include "Player.h"
#include "Engine/Runtime/Input/Input.h"
#include "Application/Utility/GameUtility.h"

#include <Engine/Assets/Animation/NodeAnimation/NodeAnimationPlayer.h>
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

void Player::initialize(const LevelLoader& level, MapchipHandler* mapchipHandler) {
	object_->reset_animated_mesh("ParentKoala.gltf", "Standby", true);
	object_->get_transform().set_translate(level.get_player_position());
	object_->set_active(true);
	object_->get_transform().set_scale(CVector3::ONE);
	object_->update_affine();

	playerState = PlayerState::Idle;

	// ビックリマークの生成
	exclamation_->reset_animated_mesh("exclamation.gltf", "Standby", false);
	exclamation_->get_animation()->set_time_force(10000);

	auto& objMat = object_->get_materials();
	for (auto& mat : objMat) {
		mat.lightingType = LighingType::None;
	}
	mapchipHandler_ = mapchipHandler;

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
	exclamation_->begin();
	flusteredEffect_->begin();
	isMove = false;
	moveNumOnIce = 1;

	isOnChild = false;
	isTurnSuccess = true;

	switch (playerState) {
	case PlayerState::Idle:
		rotateDirection = RotationDirection::Default;
		rotateType = RotateType::None;
		moveType = MoveType::Normal;
		exclamationData_.timer = 0.0f;
		break;
	case PlayerState::Moving:
		move_update();
		break;
	case PlayerState::Rotating:
		rotate_update();
		break;
	case PlayerState::MoveFailed:
		wall_move();
		break;
	case PlayerState::RotationFailed:
		rotate_failed_update();
		break;
	}


	// 子供の座標の上にビックリマークを置いておく
	exclamation_->get_transform().set_translate(object_->world_position());
	exclamation_->update();
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

void Player::move_update() {
	if (moveType != MoveType::Normal &&
		moveType != MoveType::ParentCarriesChild &&
		moveType != MoveType::ChildCarriesParent &&
		moveType != MoveType::SlidingOnIce/* &&
		moveType != MoveType::MoveOnChild*/) {
		playerState = PlayerState::MoveFailed;
		return;
	}

	//isMoving = true;

	//移動時の音。moveTimerが加算される前に処理して一回だけ鳴らす
	if (moveTimer <= 0) {
		//氷と通常床で音を変える
		if (moveType == MoveType::SlidingOnIce) {
			iceMove->restart();
		}
		else {
			moveAudio->restart();
		}
	}
	// 移動中なら補間処理を実行
	moveTimer += WorldClock::DeltaSeconds();

	// 現在の位置を補間
	Vector3 position = Vector3::Lerp(object_->get_transform().get_translate(), targetPosition, moveTimer / moveDuration);
	object_->get_transform().set_translate(position);

	// 移動中なら補間処理を実行
	if (moveType == MoveType::SlidingOnIce) {
		if (moveTimer >= moveDuration - (moveDuration * 0.30f)) {
			// 移動完了
			moveTimer = moveDuration;
			isMoving = false;
			isMoved = true;
			isMove = true;
			isOnIce = false;//この処理がupdate序盤にあると、床→氷に移動する時に音がならなかったので、ここに移動してる
			isStackMovement = true;
			playerState = PlayerState::Idle;
			object_->get_transform().set_translate(targetPosition);
			return;
		}
	}
	else {
		if (moveTimer >= moveDuration) {
			// 移動完了
			moveTimer = moveDuration;
			isMoving = false;
			isMove = true;
			isMoved = true;
			isMoving = false;
			isOnIce = false;//この処理がupdate序盤にあると、床→氷に移動する時に音がならなかったので、ここに移動してる
			isStackMovement = true;
			playerState = PlayerState::Idle;
			object_->get_transform().set_translate(targetPosition);
			return;
		}
	}

	return;
}

void Player::rotate_update() {

	if (rotateTimer <= 0) {
		rotatAudio->restart();
	}

	rotateTimer += WorldClock::DeltaSeconds();

	if (rotateType != RotateType::Normal &&
	rotateType != RotateType::Rotate90_Normal &&
		rotateType != RotateType::None) {
		playerState = PlayerState::RotationFailed;
		return;
	}

	// 回転完了チェック
	if (rotateTimer >= rotateDuration) {
		playerState = PlayerState::Moving;
		rotateType = RotateType::None;
		rotateTimer = rotateDuration;
		isRotating = false;
	}

	// 全体の進行度
	float totalProgress = rotateTimer / rotateDuration;

	Quaternion currentRotation;

	// 回転方向が逆の場合、進行度を反転し区間ごとに補間
	if (rotateDirection != RotationDirection::Default) {
		if (totalProgress <= 0.5f) {
			// 前半区間（start → mid）
			float t = totalProgress / 0.5f; // 正規化した進行度
			currentRotation = Quaternion::Slerp(startRotation, midRotation, t);
		}
		else {
			// 後半区間（mid → target）
			float t = (totalProgress - 0.5f) / 0.5f; // 正規化した進行度
			currentRotation = Quaternion::Slerp(midRotation, targetRotation, t);
		}
	}
	else {
		// 通常の回転（start → target）
		float t = totalProgress;
		currentRotation = Quaternion::Slerp(startRotation, targetRotation, t);
	}

	if (startRotation == targetRotation) {
		moveDirection = preMoveDirection;
	}

	// 現在の回転を設定
	object_->get_transform().set_quaternion(currentRotation);
}

void Player::wall_move() {
	//if (!isWallMoveing) { return; }//壁に向かって移動していない時は早期リターン
	wallMoveTimer += WorldClock::DeltaSeconds();

	Vector3 newPos = { 0,1,0 };

	//時間の半分はスタート位置から壁に向かって移動
	//多分正確に半分を計測できる訳じゃないから微妙に戻りすぎる説ある
	if (wallMoveTimer <= wallMoveDuration * 0.5f) {
		newPos = Vector3::Lerp(wallStartPosition, wallTargetPosition, wallMoveTimer / (wallMoveDuration * 0.5f));
	}
	else {
		newPos = Vector3::Lerp(wallTargetPosition, wallStartPosition, wallMoveTimer / (wallMoveDuration * 0.5f));
	}

	if (wallMoveTimer >= wallMoveDuration * 0.5f) {
		if (!unmovableFlag) {
			unmovable->restart();
			unmovableFlag = true;
		}
	}

	if (wallMoveTimer >= wallMoveDuration) {
		wallMoveTimer = wallMoveDuration;
		isWallMoveing = false;
		newPos = wallStartPosition;
		playerState = PlayerState::Idle;
		if (moveType == MoveType::MoveOnChild) {
			isMove = true;
		}

		moveType = MoveType::Normal;
		unmovableFlag = false;
	}
	object_->get_transform().set_translate(newPos);
}

void Player::rotate_failed_update() {
	// exclamation の進行度更新
	float exclamationProgress = exclamationData_.timer / exclamationData_.duration;


	// すでに待機中の場合は exclamationData_.timer だけ更新
	if (exclamationData_.isActive) {
		exclamationData_.timer += WorldClock::DeltaSeconds();

		if (exclamationProgress >= 1.0f) {
			exclamationData_.isActive = false; // 待機終了
		}
		else {
			return; // ここで処理を止める
		}
	}

	// 回転完了チェック
	if (rotateTimer >= rotateDuration) {
		playerState = PlayerState::Idle;
		rotateType = RotateType::None;
		rotateDirection = RotationDirection::Default;
		moveType = MoveType::Normal;
		rotateTimer = rotateDuration;
		exclamationData_.timer = 0.0f;
		isRotating = false;
		// 最後に目標地点の座標を入れておく
		object_->get_transform().set_quaternion(targetRotation);
		return;
	}

	// 回転のタイマーを進める
	rotateTimer += WorldClock::DeltaSeconds();
	// 全体の進行度
	float totalProgress = rotateTimer / rotateDuration;
	// 現在の回転を計算
	Quaternion currentRotation;

	// 回転方向が逆の場合、進行度を反転し区間ごとに補間
	if (rotateDirection != RotationDirection::Default) {
		if (totalProgress <= 0.5f) {
			// 前半区間（start → mid）
			float t = totalProgress / 0.5f; // 正規化した進行度
			currentRotation = Quaternion::Slerp(startRotation, midRotation, t);
			if (totalProgress >= 0.45f && exclamationProgress <= 0.3f) {
				exclamationData_.isActive = true;
				unmovable->restart();
				exclamation_->get_animation()->restart();
			}
		}
		else if (!exclamationData_.isActive) { // 待機が終わっていたら再開
			// 後半区間（mid → target）
			float t = (totalProgress - 0.5f) / 0.5f;
			currentRotation = Quaternion::Slerp(midRotation, targetRotation, t);
		}
		else {
			currentRotation = midRotation;
		}
	}

	if (startRotation == targetRotation) {
		moveDirection = preMoveDirection;
	}

	// 現在の回転を設定
	object_->get_transform().set_quaternion(currentRotation);

}
