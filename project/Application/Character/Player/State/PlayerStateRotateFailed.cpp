#include "PlayerStateRotateFailed.h"
#include <Engine/Runtime/WorldClock/WorldClock.h>
#include <Application/Character/Player/Player.h>
#include "Engine/Module/World/AnimatedMesh/AnimatedMeshInstance.h"
#include "Engine/Resources/Animation/NodeAnimation/NodeAnimationPlayer.h"

void PlayerStateRotateFailed::Enter(Player& player) {
	player.exclamation_data().timer = 0.0f;
	rotateTimer = 0.0f;
	player.set_rotating(true);
	// 移動の方向を戻しておく
	player.set_move_direction(player.get_previous_direction());
	player.set_state(PlayerState::RotationFailed);
}

void PlayerStateRotateFailed::Update(Player& player) {
	// exclamation の進行度更新
	float exclamationProgress = player.exclamation_data().timer / player.exclamation_data().duration;

	// すでに待機中の場合は exclamationData_.timer だけ更新
	if (player.exclamation_data().isActive) {
		player.exclamation_data().timer += WorldClock::DeltaSeconds();

		if (exclamationProgress >= 1.0f) {
			player.exclamation_data().isActive = false; // 待機終了
		}
		else {
			return; // ここで処理を止める
		}
	}

	// 回転完了チェック
	if (rotateTimer >= rotateDuration) {
		player.change_state("Idle");
		player.set_rotate_type(RotateType::None);
		player.set_how_rotation(RotationDirection::Default);
		return;
	}

	// 回転のタイマーを進める
	rotateTimer += WorldClock::DeltaSeconds();
	// 全体の進行度
	float totalProgress = rotateTimer / rotateDuration;
	// 現在の回転を計算
	Quaternion currentRotation;

	// 回転方向が逆の場合、進行度を反転し区間ごとに補間
	//if (player.get_how_rotation() != RotationDirection::Default) {
		if (totalProgress <= 0.5f) {
			// 前半区間（start → mid）
			float t = totalProgress / 0.5f; // 正規化した進行度
			currentRotation = Quaternion::Slerp(player.get_start_rotation(), player.get_mid_rotation(), t);
			if (totalProgress >= 0.40f && exclamationProgress <= 0.3f) {
				player.exclamation_data().isActive = true;
				//unmovable->restart();
				player.exclamation_data().exclamation_->get_animation()->restart();
			}
		}
		else if (!player.exclamation_data().isActive) { // 待機が終わっていたら再開
			// 後半区間（mid → target）
			float t = (totalProgress - 0.5f) / 0.5f;
			currentRotation = Quaternion::Slerp(player.get_mid_rotation(), player.get_target_rotation(), t);
		}
		else {
			currentRotation = player.get_mid_rotation();
		}
	//}

	// 現在の回転を設定
	player.set_rotation(currentRotation);
}

void PlayerStateRotateFailed::Exit(Player& player) {


	player.set_rotating(false);
	// 最後に目標地点の座標を入れておく
	player.set_rotation(player.get_target_rotation());
}
