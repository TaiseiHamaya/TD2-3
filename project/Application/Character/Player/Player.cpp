#include "Player.h"
#include "Engine/Runtime/Input/Input.h"
#include "Application/Utility/GameUtility.h"

#include <Engine/Resources/Animation/NodeAnimation/NodeAnimationPlayer.h>

void Player::initialize(const LevelLoader& level, MapchipHandler* mapchipHandler) {
	object_ = std::make_unique<AnimatedMeshInstance>();
	object_->reset_animated_mesh("ParentKoala.gltf", "Standby", true);
	object_->get_transform().set_translate(level.get_player_position());
	mapchipHandler_ = mapchipHandler;

	//音関連

	moveAudio = std::make_unique<AudioPlayer>();
	moveAudio->initialize("move.wav");
	unmovable = std::make_unique<AudioPlayer>();
	unmovable->initialize("unmovable.wav");
	fall = std::make_unique<AudioPlayer>();
	fall->initialize("fall.wav");
	iceMove = std::make_unique<AudioPlayer>();
	iceMove->initialize("iceMove.wav");
	rotatAudio = std::make_unique<AudioPlayer>();
	rotatAudio->initialize("rotate.wav");

	fallSoundFlag = false;
	unmovableFlag = false;
}

void Player::finalize() {}

void Player::update() {
	isStackMovement = false;
	object_->begin();
	isMove = false;
	moveNumOnIce = 1;
	
	isOnChild = false;

	// 入力処理
	handle_input();

	if (isRotating) {
		rotate_update();
	}
	else {
		move_update();
		wall_move();
	}

	fall_update();

	object_->update();

	preDirection = direction;
}

void Player::begin_rendering() {
	object_->begin_rendering();
}

void Player::draw() const {
	object_->draw();
}

void Player::on_undo(Vector3 position, Quaternion rotation, bool setParent) {
	object_->get_transform().set_translate(position);
	object_->get_transform().set_quaternion(
		rotation
	);
	direction = -CVector3::BASIS_Z * rotation;
	isParent = setParent;
}

#ifdef _DEBUG
void Player::debug_update() {
	ImGui::Begin("Player");
	ImGui::Text("%d", isParent);
	object_->debug_gui();
	ImGui::End();
}
#endif // _DEBUG

void Player::handle_input() {
	if (isMoving) return; // 移動中は入力を無視
	if (isRotating) return; // 回転中は入力を無視
	if (isWallMoveing) { return; }//壁に向かって移動している時は早期リターン

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
				if (!isOnChild) {
					targetPosition = nextPosition;
				}
				startRotation = object_->get_transform().get_quaternion();
				moveTimer = 0.0f;
				moveDuration = 0.15f;
				moveStartPosition = object_->get_transform().get_translate();
				isMoving = true;
			}
			// 進行先が氷かどうかチェック
			else if (mapchipHandler_->can_player_move_on_ice(this, child_, direction)) {
				targetPosition = get_translate() + direction * static_cast<float>(moveNumOnIce);
				moveTimer = 0.0f;
				moveDuration = 0.15f * static_cast<float>(moveNumOnIce);
				isOnIce = true;
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

			//進行先が壁か穴かを判定
			// 回転途中に壁がある時に上手く回転しないバグによってこっちもバグってる
			//条件を「入力方向に回転する時引っかからない」かつ「進行方向が壁か穴」だと上手く行く
			if (mapchipHandler_->player_move_to_wall_or_holl(this, child_, direction)) {
				wallTargetPosition = get_translate() + direction * 0.5f;
				wallMoveTimer = 0.0f;
				wallMoveDuration = 0.15f;
				isWallMoveing = true;
				wallStartPosition = object_->get_transform().get_translate();
			}

			break;
		}
	}
}

void Player::fall_update() {
	if (isFalling) {
		Vector3 position = object_->get_transform().get_translate();
		if (!fallSoundFlag) {
			fall->play();
			fallSoundFlag = true;
		}

		position.y -= fallSpeed * WorldClock::DeltaSeconds();
		object_->get_transform().set_translate(position);
		if (position.y <= -3.0f) {
			isFalled = true;
		}
	}
}

void Player::move_update() {
	if (!isMoving) {
		targetPosition = object_->get_transform().get_translate();
		return;
	};

	//移動時の音。moveTimerが加算される前に処理して一回だけ鳴らす
	if (moveTimer <= 0) {
		//氷と通常床で音を変える
		if (isOnIce) {
			iceMove->restart();
		}
		else {
			moveAudio->restart();

		}
	}
	// 移動中なら補間処理を実行
	moveTimer += WorldClock::DeltaSeconds();

	if (moveTimer >= moveDuration) {
		// 移動完了
		moveTimer = moveDuration;
		isMoving = false;
		isMove = true;
		isOnIce = false;//この処理がupdate序盤にあると、床→氷に移動する時に音がならなかったので、ここに移動してる
		isStackMovement = true;
	}

	// 現在の位置を補間
	Vector3 position = Vector3::Lerp(object_->get_transform().get_translate(), targetPosition, moveTimer / moveDuration);
	object_->get_transform().set_translate(position);
	return;
}

void Player::rotate_update() {

	if (rotateTimer <= 0) {
		rotatAudio->restart();
	}

	rotateTimer += WorldClock::DeltaSeconds();

	// 回転完了チェック
	if (rotateTimer >= rotateDuration) {
		rotateTimer = rotateDuration;
		isRotating = false;
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
	if (startRotation == targetRotation) {
		direction = preDirection;
	}

	// 現在の回転を設定
	object_->get_transform().set_quaternion(currentRotation);
}

void Player::wall_move() {
	if (!isWallMoveing) { return; }//壁に向かって移動していない時は早期リターン
	wallMoveTimer += WorldClock::DeltaSeconds();


	Vector3 newPos = { 0,0,0 };


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
		unmovableFlag = false;
	}
	object_->get_transform().set_translate(newPos);
}
