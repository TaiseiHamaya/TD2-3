#include "Player.h"
#include "Engine/Runtime/Input/Input.h"
#include "Application/Utility/GameUtility.h"

#include <Engine/Resources/Animation/NodeAnimation/NodeAnimationPlayer.h>

void Player::initialize(const LevelLoader& level, MapchipHandler* mapchipHandler) {
	object_ = std::make_unique<AnimatedMeshInstance>();
	object_->reset_animated_mesh("ParentKoala.gltf", "Standby", true);
	object_->get_transform().set_translate(level.get_player_position());
	mapchipHandler_ = mapchipHandler;
}

void Player::finalize() {}

void Player::update() {
	isStackMovement = false;
	object_->begin();
	isMove = false;
	moveNumOnIce = 1;
	isOnIce = false;
	isOnChild = false;
	isTurnSuccess = true;

	switch (playerState) {
	case PlayerState::Idle:
		rotateDirection = RotationDirection::Default;
		rotateType = RotateType::None;
		moveType = MoveType::Normal;
		break;
	case PlayerState::Falling:
		fall_update();
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

	object_->update();

	preMoveDirection = moveDirection;
}

void Player::begin_rendering() {
	object_->begin_rendering();
}

void Player::draw() const {
	object_->draw();
}

void Player::on_undo(Vector3 position, Quaternion rotation, bool setParent, bool onGround) {
	object_->get_transform().set_translate(position);
	object_->get_transform().set_quaternion(
		rotation
	);
	moveDirection = -CVector3::BASIS_Z * rotation;
	isParent = setParent;
	if (isParent) {
		if (onGround) {
			object_->get_animation()->reset_animation("Standby");
		}
		else {
			object_->get_animation()->reset_animation("Flustered");
		}
	}
	else {
		object_->get_animation()->reset_animation("Standby");
	}
}

#ifdef _DEBUG
void Player::debug_update() {
	ImGui::Begin("Player");
	object_->debug_gui();
	ImGui::End();
}
#endif // _DEBUG


void Player::fall_update() {
	Vector3 position = object_->get_transform().get_translate();
	position.y -= fallSpeed * WorldClock::DeltaSeconds();
	object_->get_transform().set_translate(position);
	if (position.y <= -3.0f) {
		isFalled = true;
	}
}

void Player::move_update() {
	// 移動中なら補間処理を実行
	moveTimer += WorldClock::DeltaSeconds();

	if (moveTimer >= moveDuration) {
		// 移動完了
		playerState = PlayerState::Idle;
		moveTimer = moveDuration;
		isMoving = false;
		isMove = true;
		isStackMovement = true;
	}

	// 現在の位置を補間
	Vector3 position = Vector3::Lerp(object_->get_transform().get_translate(), targetPosition, moveTimer / moveDuration);
	object_->get_transform().set_translate(position);
	return;
}

void Player::rotate_update() {
	rotateTimer += WorldClock::DeltaSeconds();

	if (rotateType != RotateType::Normal &&
	rotateType != RotateType::Rotate90_Normal &&
	rotateType != RotateType::None) {
		playerState = PlayerState::RotationFailed;
	}

	// 回転完了チェック
	if (rotateTimer >= rotateDuration) {
		playerState = PlayerState::Moving;
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

	if (wallMoveTimer >= wallMoveDuration) {
		wallMoveTimer = wallMoveDuration;
		isWallMoveing = false;
		newPos = wallStartPosition;
		playerState = PlayerState::Idle;
	}
	//object_->get_transform().set_translate(newPos);
}

void Player::rotate_failed_update() {
	rotateTimer += WorldClock::DeltaSeconds();

	// 回転完了チェック
	if (rotateTimer >= rotateDuration) {
		playerState = PlayerState::MoveFailed;
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
