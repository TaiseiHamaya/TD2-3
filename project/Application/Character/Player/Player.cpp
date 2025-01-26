#include "Player.h"
#include "Engine/Runtime/Input/Input.h"
#include "Application/Utility/GameUtility.h"

void Player::initialize(const LevelLoader& level, MapchipHandler* mapchipHandler)
{
	object_ = std::make_unique<MeshInstance>();
	object_->reset_mesh("ParentKoala.obj");
	object_->get_transform().set_translate(level.get_player_position());
	mapchipHandler_ = mapchipHandler;
}

void Player::finalize()
{
}

void Player::update()
{
	isMove = false;
	moveNumOnIce = 1;

	// 入力処理
	handle_input();

	if (isRotating) {
		rotate_update();
	}
	else {
		move_update();
	}

	fall_update();

	preDirection = direction;
}

void Player::begin_rendering()
{
	object_->begin_rendering();
}

void Player::draw() const
{
	object_->draw();
}

#ifdef _DEBUG
void Player::debug_update()
{
	ImGui::Begin("Player");
	object_->debug_gui();
	ImGui::End();
}
#endif // _DEBUG

void Player::handle_input()
{
	if (isMoving) return; // 移動中は入力を無視

	Vector3 directions[] = {
		{0.0f, 0.0f, 1.0f},  // 前
		{-1.0f, 0.0f, 0.0f}, // 左
		{0.0f, 0.0f, -1.0f}, // 後ろ
		{1.0f, 0.0f, 0.0f}   // 右
	};

	KeyID keys[] = { KeyID::W, KeyID::A, KeyID::S, KeyID::D };

	for (size_t i = 0; i < 4; ++i) {
		if (Input::GetInstance().IsTriggerKey(keys[i])) {
			direction = directions[i];
			Vector3 nextPosition = get_translate() + direction;

			// MapchipHandlerに移動可能かを問い合わせ
			if (mapchipHandler_->can_player_move_to(this, child_, direction)) {
				targetPosition = nextPosition;
				moveTimer = 0.0f;
				moveDuration = 0.15f;
				isMoving = true;
			}else 
				// 進行先が氷かどうかチェック
			if (mapchipHandler_->can_player_move_on_ice(this, child_, direction)) {
				if (moveNumOnIce == 0) {
					return;
				}
				targetPosition = get_translate() + direction * static_cast<float>(moveNumOnIce);
				moveTimer = 0.0f;
				moveDuration = 0.15f * static_cast<float>(moveNumOnIce);
				isMoving = true;
			}

			// 回転中の衝突チェック
			if (mapchipHandler_->can_player_rotate(this, child_, direction)) {
				startRotation = object_->get_transform().get_quaternion();
				targetRotation = Quaternion::FromToRotation({ 0.0f, 0.0f, -1.0f }, direction);
				midRotation = targetRotation;
				rotateTimer = 0.0f;
				isRotating = true;
				isReverseRotation = false;

				mapchipHandler_->setup_rotation_parameters(this, child_, direction);
			}
			else {
				// プレイヤーの向いている方向を前フレームの物に戻す
				direction = preDirection;
				rotateTimer = 0.0f;
				isRotating = false;
			}

			break;
		}
	}
}

void Player::fall_update()
{
	if (isFalling) {
		Vector3 position = object_->get_transform().get_translate();
		position.y -= fallSpeed * WorldClock::DeltaSeconds();
		object_->get_transform().set_translate(position);
	}
}

void Player::move_update()
{
	if (!isMoving) { 
		targetPosition = object_->get_transform().get_translate();
		return;
	};

	// 移動中なら補間処理を実行
	moveTimer += WorldClock::DeltaSeconds();

	if (moveTimer >= moveDuration) {
		// 移動完了
		moveTimer = moveDuration;
		isMoving = false;
		isMove = true;
	}

	// 現在の位置を補間
	Vector3 position = Vector3::Lerp(object_->get_transform().get_translate(), targetPosition, moveTimer / moveDuration);
	object_->get_transform().set_translate(position);
	return;
}

void Player::rotate_update()
{
	rotateTimer += WorldClock::DeltaSeconds();

	// 回転完了チェック
	if (rotateTimer >= rotateDuration) {
		rotateTimer = rotateDuration;
		isRotating = false;
		isMoving = true;
	}

	// 全体の進行度
	float totalProgress = rotateTimer / rotateDuration;

	Quaternion currentRotation;

	// 回転方向が逆の場合、進行度を反転し区間ごとに補間
	if (isReverseRotation) {
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

	// 現在の回転を設定
	object_->get_transform().set_quaternion(currentRotation);
}
