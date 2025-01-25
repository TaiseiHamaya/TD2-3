#pragma once
#include "Application/Character/CharacterBase.h"

class Child : public CharacterBase {
public:
    void initialize(const LevelLoader& level) override;
    void finalize() override;
    void update() override;
    void begin_rendering() override;
    void draw() const override;

    void set_parent(MeshInstance* parent) { object_->reparent(parent); }
    void unset_parent() { object_->reparent(nullptr); }
    void set_falling(bool flag) { isFall = flag; }

#ifdef _DEBUG
    void debug_update();
#endif
private:
    bool isFall = false;
};
