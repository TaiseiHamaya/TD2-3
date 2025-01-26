#pragma once
#include "Application/Character/CharacterBase.h"
#include "Engine/Runtime/WorldClock/WorldClock.h"
#include <Application/Mapchip/MapchipHandler.h>


class Player : public CharacterBase {
public:
	void initialize(const LevelLoader& level, MapchipHandler* mapchipHandler) override;
	void finalize() override;
	void update() override;

	void begin_rendering() override;
	void draw() const override;

	// アクセッサ
	void set_child(Child* child) { child_ = child; }

	Vector3 get_translate() const { return object_->get_transform().get_translate(); }
	void set_translate(const Vector3& position) { object_->get_transform().set_translate(position); }

	Quaternion get_rotation() const { return object_->get_transform().get_quaternion(); }
	void set_rotation(const Quaternion& rotation) { object_->get_transform().set_quaternion(rotation); }

	bool is_parent() const { return isParent; }
	void set_parent(bool flag) { isParent = flag; }

	bool is_falling() const { return isFalling; }
	void set_falling(bool flag) { isFalling = flag; }

	bool is_moving() const { return isMoving; }
	void set_moving(bool flag) { isMoving = flag; }

	bool is_moved() const { return isMove; }

	Vector3 get_previous_direction() const { return preDirection; }

	void set_reverse_rotation(bool flag) { isReverseRotation = flag; }

	void set_mid_rotation(Quaternion Rotation) { midRotation = Rotation; }

#ifdef _DEBUG
	void debug_update();
#endif
private:

	void handle_input();
	void fall_update();
	void move_update();
	void rotate_update();

private:
	MapchipHandler* mapchipHandler_;
	Child* child_; // 子オブジェクトへの参照

	bool isMove = false; // 今フレームで移動をしたかどうか
	bool isParent = false; // 子供を持つかどうか
	bool isFalling = false; // 落下中かどうか
	Vector3 direction{}; // 移動方向
	Vector3 preDirection{}; // 移動方向

	Vector3 startPosition;   // 移動の開始位置
	Vector3 targetPosition;  // 次の目標位置
	float moveTimer = 0.0f;  // 移動の進行状況を管理するタイマー
	float moveDuration = 0.15f;  // 移動にかける時間（秒）
	bool isMoving = false;   // 現在移動中かどうかのフラグ

	Quaternion targetRotation;  // 次の目標回転
	Quaternion startRotation;   // 補間の開始回転
	Quaternion midRotation;     // 180度回転するときの中間点
	float rotateTimer = 0.0f;    // 回転の進行状況を管理するタイマー
	float rotateDuration = 0.3f; // 回転にかける時間（秒）
	bool isRotating = false;     // 現在回転中かどうかのフラグ
	bool isReverseRotation = false; // 回転を反転

	float fallSpeed = 6.0f;//落下中の速度

	float deltaTime = WorldClock::DeltaSeconds(); // 時間管理
};