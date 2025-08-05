#pragma once
#include <memory>
#include "Engine/Module/World/AnimatedMesh/AnimatedMeshInstance.h"
#include "Application/LevelLoader/LevelLoader.h"
#include "Application/Mapchip/MapchipHandler.h"


class CharacterBase {
public:
    virtual ~CharacterBase() = default;

    virtual void initialize(const LevelLoader& level, MapchipField* mapchipField) = 0;
    virtual void finalize() = 0;
    virtual void update() = 0;
    virtual void begin_rendering() = 0;
    virtual void draw() const = 0;

    Vector3 get_translate() const { return object_->get_transform().get_translate(); }
    void set_translate(const Vector3& position) { object_->get_transform().set_translate(position); }

    Quaternion get_rotation() const { return object_->get_transform().get_quaternion(); }
    void set_rotation(const Quaternion& rotation) { object_->get_transform().set_quaternion(rotation); }

	AnimatedMeshInstance* get_object() const { return object_.get(); }

    bool is_falling() const { return isFalling; }
    void set_falling(bool flag) { isFalling = flag; }
    // 地面の上にいるかの判定
    bool is_out_ground() const { return isOutGround; }
    void set_out_ground(bool flag) { isOutGround = flag; }

protected:
    std::unique_ptr<AnimatedMeshInstance> object_;

    bool isFalling = false; // 現在落下中かどうかのフラグ
    float fallSpeed = 6.0f; // 落下速度
    bool isFalled = false; // 落下終了フラグ

    // 地面の上にいるかの判定するフラグ
    bool isOutGround = false;


};