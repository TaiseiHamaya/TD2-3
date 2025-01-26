#pragma once
#include "Application/Character/CharacterBase.h"
#include "Engine/Runtime/WorldClock/WorldClock.h"
#include <Application/Mapchip/MapchipHandler.h>

class Child : public CharacterBase {
public:
    void initialize(const LevelLoader& level, MapchipHandler* mapchipHandler) override;
    void finalize() override;
    void update() override;
    void begin_rendering() override;
    void draw() const override;


    Quaternion get_rotation() const { return object_->get_transform().get_quaternion(); }
    void set_rotation(const Quaternion& rotation) { object_->get_transform().set_quaternion(rotation); }
    void set_parent(MeshInstance* parent) { object_->reparent(parent); }
    void unset_parent() { object_->reparent(nullptr); }
    void set_falling(bool flag) { isFall = flag; }

#ifdef _DEBUG
    void debug_update();
#endif
private:
    bool isFall = false;
    float fallSpeed = 6.0f;//落下速度
};
