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

    void set_parent(MeshInstance* parent) { object_->reparent(parent); }
    void unset_parent() { object_->reparent(nullptr); }



    bool is_falled()const { return isFalled; }

#ifdef _DEBUG
    void debug_update();
#endif
private:


};
