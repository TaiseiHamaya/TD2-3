#pragma once
#include "Application/Character/CharacterBase.h"
#include <Application/MapchipField.h>
#include "Engine/Runtime/WorldClock/WorldClock.h"
#include "Application/Character/Player/Child/Child.h"

class Player : public CharacterBase {
public:
    void initialize() override;
    void finalize() override;
    void update() override;
    void update(MapchipField* mapchipField);
    void begin_rendering() override;
    void draw() const override;

#ifdef _DEBUG
    void debug_update();
#endif
    // アクセッサ
    Vector3 get_translate_instance() const { return targetPosition; }
    Vector3 get_direction() const { return direction; }

    void set_child(CharacterBase* child) { child_ = child; }
    bool is_parent() const { return isParent; }
    void set_parent(bool flag) { isParent = flag; }
    void set_falling(bool flag) { isFall = flag; }
    bool is_moving() const { return isMove; }
    void cancel_rotation(bool flag) { isCancelRotate = flag; }

private:
    void move(MapchipField* mapchipField);
    void rotate(MapchipField* mapchipField);
    bool can_move_to(const Vector3& position, MapchipField* mapchipField) const;
    void start_rotation(MapchipField* mapchipField);
    bool check_collision_during_rotation(MapchipField* mapchipField);
    bool check_collision_during_translation(MapchipField* mapchipField);
    bool approximately_equal(const Vector3& a, const Vector3& b, float epsilon = 1e-5f);
    Vector3 rotate_direction_90_left(const Vector3& direction);
    Vector3 rotate_direction_90_right(const Vector3& direction);

private:
    CharacterBase* child_;

    bool isParent = false;
    bool isMove = false;
    bool isFall = false;
    Vector3 direction{};
    Vector3 preDirection{};

    Vector3 targetPosition;  // 次の目標位置
    float moveTimer = 0.0f;  // 移動の進行状況を管理するタイマー
    float moveDuration = 0.3f;  // 移動にかける時間（秒）
    bool isMoving = false;   // 現在移動中かどうかのフラグ

    Quaternion targetRotation;  // 次の目標回転
    Quaternion startRotation;   // 補間の開始回転
    Quaternion midRotation;     // 180度回転するときの中間点
    float rotateTimer = 0.0f;    // 回転の進行状況を管理するタイマー
    float rotateDuration = 0.3f; // 回転にかける時間（秒）
    bool isRotating = false;     // 現在回転中かどうかのフラグ
    bool isReverseRotation = false; // 回転を反転

    bool isCancelRotate = false; // 回転が途中でさえぎられた場合の処理

    float deltaTime = WorldClock::DeltaSeconds();
};
