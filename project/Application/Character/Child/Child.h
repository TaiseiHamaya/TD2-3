#pragma once
#include "Application/Character/CharacterBase.h"
#include "Engine/Runtime/WorldClock/WorldClock.h"
#include <Application/Mapchip/MapchipHandler.h>
#include "Engine/Resources/Audio/AudioPlayer.h"


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

    bool is_falling() const { return isFall; }
    void set_falling(bool flag) { isFall = flag; }

    bool is_out_ground() const { return isOutGround; }
    void set_out_ground(bool flag) { isOutGround = flag; }

    bool is_falled()const { return isFalled; }

	void on_undo(Vector3 position, Vector3 lookat);

#ifdef _DEBUG
    void debug_update();
#endif
private:
    bool isFall = false;
    float fallSpeed = 6.0f;//落下速度
    bool isFalled = false;
    // 
    bool isOutGround = false; 

    //音関連


};
