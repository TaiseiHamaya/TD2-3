#pragma once
#include "Application/Character/CharacterBase.h"
#include "Engine/Runtime/WorldClock/WorldClock.h"
#include <Application/Mapchip/MapchipHandler.h>
#include "CharacterStates.h"
#include "Engine/Resources/Audio/AudioPlayer.h"
#include <Engine/Module/World/Particle/ParticleEmitterInstance.h>
#include <memory>


class Player : public CharacterBase {
public:
	void initialize(const LevelLoader& level, MapchipHandler* mapchipHandler) override;
	void finalize() override;
	void update() override;

	void begin_rendering() override;
	void draw() const override;

public: // アクセッサ
	void set_child(Child* child) { child_ = child; }

	bool is_parent() const { return isParent; }
	void set_parent(bool flag) { isParent = flag; }

	bool is_moving() const { return isMoving; }
	void set_moving(bool flag) { isMoving = flag; }

	bool is_rotating() const { return isRotating; }
	void set_rotating(bool flag) { isRotating = flag; }

	void set_rotate_timer(float time) { rotateTimer = time; }
	void set_rotate_duration(float time) { rotateDuration = time; }

	bool is_on_ice() const { return isOnIce; }
	void set_on_ice(bool flag) { isOnIce = flag; }

	int is_parent_on_ice() const { return isParentOnIce; }
	void set_parent_on_ice(int index) { isParentOnIce = index; }

	bool is_moved() const { return isMove; }
	void set_moved(bool flag) { isMove = flag; }

	bool is_on_child() const { return isOnChild; }
	void set_on_child(bool flag) { isOnChild = flag; }

	bool is_turn_success() const { return isTurnSuccess; }
	void set_turn_success(bool flag) { isTurnSuccess = flag; }

	Vector3 get_previous_direction() const { return preMoveDirection; }

	RotationDirection get_how_rotation() const { return rotateDirection; }
	void set_how_rotation(RotationDirection rotate) { rotateDirection = rotate; }

	PlayerState get_state() const { return playerState; }
	void set_state(PlayerState state) { playerState = state; }

	MoveType get_move_type() const { return moveType; }
	void set_move_type(MoveType type) { moveType = type; }

	RotateType get_rotate_type() const { return rotateType; }
	void set_rotate_type(RotateType type) { rotateType = type; }

	void set_mid_rotation(Quaternion Rotation) { midRotation = Rotation; }

	Quaternion get_start_rotation() const { return startRotation; }
	void set_start_rotation(Quaternion rotation) { startRotation = rotation; }

	Quaternion get_target_rotation() const { return targetRotation; }
	void set_target_rotation(Quaternion rotation) { targetRotation = rotation; }

	int get_move_num_on_ice() const { return moveNumOnIce; }
	void set_move_num_on_ice(int num) { moveNumOnIce = num; }

	bool is_falled()const { return isFalled; }

	bool is_stack_movement() const { return isStackMovement; }
	void on_undo(Vector3 position, Quaternion direction, bool isParent);
	Vector3 move_start_position() const { return moveStartPosition; }
	void set_start_position(const Vector3& pos) { moveStartPosition = pos; }

	//Vector3 get_target_pos() const { return targetPosition; }
	void get_target_pos(const Vector3& pos) { targetPosition = pos; }

	Quaternion start_rotation() const { return startRotation; }

	void set_move_duration(float time) { moveDuration = time; };
	void set_move_timer(float timer) { moveTimer = timer; };

	void set_wall_start_pos(const Vector3& pos) { wallStartPosition = pos; }
	void set_wall_target_pos(const Vector3& pos) { wallTargetPosition = pos; }
	void set_wall_timer(float timer) { wallMoveTimer = timer; }
	void set_wall_duration(float time) { wallMoveDuration = time; }
	void set_wall_moving(bool flag) { isWallMoveing = flag; }

#ifdef _DEBUG
	void debug_update();
#endif
private:

	void fall_update();
	void move_update();
	void rotate_update();
	void wall_move();
	void rotate_failed_update();

private:
	// 回転の仕方をまとめたenum
	RotationDirection rotateDirection;
	// プレイヤーの状態
	PlayerState playerState;
	// なぜ回転が失敗したのか
	RotateType rotateType;
	// 橋渡しみたいに移動するか
	MoveType moveType;

	MapchipHandler* mapchipHandler_;
	Child* child_; // 子オブジェクトへの参照

	bool isMove = false; // 今フレームで移動をしたかどうか
	bool isParent = false; // 子供を持つかどうか
	Vector3 moveDirection{}; // 移動方向
	Vector3 preMoveDirection{}; // 前回の移動方向を保存

	Vector3 moveStartPosition;   // 移動の開始位置
	Vector3 targetPosition;  // 次の目標位置
	float moveTimer = 0.0f;  // 移動の進行状況を管理するタイマー
	float moveDuration = 0.15f;  // 移動にかける時間（秒）
	bool isMoving = false;   // 現在移動中かどうかのフラグ
	bool isOnChild = false; // 移動先が子供かどうか

	bool isTurnSuccess; // 回転が成功したかどうか

	Vector3 wallStartPosition;   // 移動の開始位置
	Vector3 wallTargetPosition;  // 次の目標位置
	float wallMoveTimer = 0.0f; //現在の移動時間
	float wallMoveDuration = 0.15f; // 移動にかける時間（秒）
	bool isWallMoveing = false; // 壁に向かって移動した時のフラグ



	int moveNumOnIce = 1; // 氷の上を何マス
	bool isOnIce = false; // 氷の上を移動するか否か
	int isParentOnIce = 0; // 氷上での子供との接触フラグ(0:無し 1:正面 2:左 3:右)

	Quaternion targetRotation;  // 次の目標回転
	Quaternion startRotation;   // 補間の開始回転
	Quaternion midRotation;     // 180度回転するときの中間点
	float rotateTimer = 0.0f;    // 回転の進行状況を管理するタイマー
	float rotateDuration = 0.3f; // 回転にかける時間（秒）
	bool isRotating = false;     // 現在回転中かどうかのフラグ
	bool canRotate = false; // 回転可能かどうかのフラグ


	bool isStackMovement{ false };

	float deltaTime = WorldClock::DeltaSeconds(); // 時間管理

	//音関連
	std::unique_ptr<AudioPlayer> moveAudio;//通常移動
	std::unique_ptr<AudioPlayer> unmovable;//動けない所に移動しようとしてる音。穴や壁で移動がキャンセルされる時に鳴らす
	bool unmovableFlag;
	std::unique_ptr<AudioPlayer> fall;//落下音
	bool fallSoundFlag;
	std::unique_ptr<AudioPlayer> iceMove;//通常移動
	std::unique_ptr<AudioPlayer> rotatAudio;//回転時間

};