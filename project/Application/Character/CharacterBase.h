#pragma once
#include <memory>
#include "Engine/Module/World/AnimatedMesh/AnimatedMeshInstance.h"
#include "Application/LevelLoader/LevelLoader.h"
#include "Application/Mapchip/MapchipHandler.h"

class CharacterBase {
public:
    virtual ~CharacterBase() = default;

    virtual void initialize(const LevelLoader& level, MapchipHandler* mapchipHandler) = 0;
    virtual void finalize() = 0;
    virtual void update() = 0;
    virtual void begin_rendering() = 0;
    virtual void draw() const = 0;

    Vector3 get_translate() const { return object_->get_transform().get_translate(); }
    void set_translate(const Vector3& translate) { object_->get_transform().set_translate(translate); }

    Quaternion get_rotation() const { return object_->get_transform().get_quaternion(); }
	AnimatedMeshInstance* get_object() const { return object_.get(); }

protected:
    std::unique_ptr<AnimatedMeshInstance> object_;
};