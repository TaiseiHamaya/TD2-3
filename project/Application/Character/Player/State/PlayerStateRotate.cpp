#include "PlayerStateRotate.h"
#include <Library/Math/Quaternion.h>
#include <Application/Character/Player/Player.h>

PlayerStateRotate::PlayerStateRotate() {
	rotatAudio = std::make_unique<AudioPlayer>();
	rotatAudio->initialize("rotate.wav");
}

void PlayerStateRotate::Enter(Player& player) {
	rotateTimer = 0.0f;
	player.set_rotating(true);
	player.set_state(PlayerState::Rotating);
}

void PlayerStateRotate::Update(Player& player) {
	if (rotateTimer <= 0) {
		rotatAudio->restart();
	}

	rotateTimer += WorldClock::DeltaSeconds();

	//if (player.get_rotate_type() != RotateType::Normal &&
	//player.get_rotate_type() != RotateType::Rotate90_Normal &&
	//	player.get_rotate_type() != RotateType::None) {
	//	playerState = PlayerState::RotationFailed;
	//	return;
	//}

	// 回転完了チェック
	if (rotateTimer >= rotateDuration) {
		//playerState = PlayerState::Moving;
		player.change_state("MoveCheck");
		//isRotating = false;
		return;
	}

	// 全体の進行度
	float totalProgress = rotateTimer / rotateDuration;

	Quaternion currentRotation;

	// 回転方向が逆の場合、進行度を反転し区間ごとに補間
	if (player.get_how_rotation() != RotationDirection::Default) {
		if (totalProgress <= 0.5f) {
			// 前半区間（start → mid）
			float t = totalProgress / 0.5f; // 正規化した進行度
			currentRotation = Quaternion::Slerp(player.get_start_rotation(), player.get_mid_rotation(), t);
		}
		else {
			// 後半区間（mid → target）
			float t = (totalProgress - 0.5f) / 0.5f; // 正規化した進行度
			currentRotation = Quaternion::Slerp(player.get_mid_rotation(), player.get_target_rotation(), t);
		}
	}
	else {
		// 通常の回転（start → target）
		float t = totalProgress;
		currentRotation = Quaternion::Slerp(player.get_start_rotation(), player.get_target_rotation(), t);
	}

	//// 回転が変わってない場合移動の方向を戻しておく
	//if (player.get_start_rotation() == player.get_target_rotation()) {
	//	player.set_move_direction(player.get_previous_direction());
	//}

	// 現在の回転を設定
	//player.get_object()->get_transform().set_quaternion(currentRotation);
	player.set_rotation(currentRotation);
	
}

void PlayerStateRotate::Exit(Player& player) {
	player.set_rotate_type(RotateType::None);
	player.set_rotating(false);

	player.set_rotation(player.get_target_rotation());
}
