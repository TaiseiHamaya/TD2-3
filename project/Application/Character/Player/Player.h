#pragma once
#include "CharacterBase.h"
#include <Application/MapchipField.h>
#include "Engine/Runtime/WorldClock/WorldClock.h"

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

    bool is_parent() const { return isParent; }
    void set_parent(bool flag) { isParent = flag; }
    void set_falling(bool flag) { isFall = flag; }
    bool is_moving() const { return isMove; }

private:
    void move(MapchipField* mapchipField);
    void rotate();
    bool can_move_to(const Vector3& position, MapchipField* mapchipField) const;
    void start_rotation(const Vector3& direction);

private:
    bool isParent = false;
    bool isMove = false;
    bool isFall = false;
    Vector3 direction{};

    Vector3 targetPosition;  // 次の目標位置
    float moveTimer = 0.0f;  // 移動の進行状況を管理するタイマー
    float moveDuration = 0.3f;  // 移動にかける時間（秒）
    bool isMoving = false;   // 現在移動中かどうかのフラグ

    Quaternion targetRotation;  // 次の目標回転
    Quaternion startRotation;   // 補間の開始回転
    float rotateTimer = 0.0f;    // 回転の進行状況を管理するタイマー
    float rotateDuration = 0.3f; // 回転にかける時間（秒）
    bool isRotating = false;     // 現在回転中かどうかのフラグ

    float deltaTime = WorldClock::DeltaSeconds();
};
