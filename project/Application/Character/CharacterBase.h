#pragma once
#include <memory>
#include "Engine/Module/World/Mesh/MeshInstance.h"
#include "Application/LevelLoader/LevelLoader.h"

class CharacterBase {
public:
    virtual ~CharacterBase() = default;

    virtual void initialize(const LevelLoader& level) = 0;
    virtual void finalize() = 0;
    virtual void update() = 0;
    virtual void begin_rendering() = 0;
    virtual void draw() const = 0;

    Vector3 get_translate() const { return object_->get_transform().get_translate(); }
    void set_translate(const Vector3& translate) { object_->get_transform().set_translate(translate); }

    Quaternion get_rotation() const { return object_->get_transform().get_quaternion(); }
    MeshInstance* get_object() const { return object_.get(); }

protected:
    std::unique_ptr<MeshInstance> object_;
};