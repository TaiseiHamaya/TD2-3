#pragma once
#include <memory>
#include "Application/Character/Player/Player.h"
#include "Application/Character/Player/Child/Child.h"
#include <Application/MapchipField.h>

class PlayerManager {
public:
    void initialize(const LevelLoader& level);
    void finalize();
    void update();
    void begin_rendering();
    void draw() const;

#ifdef _DEBUG
    void debug_update();
#endif

    void set_mapchip_field(MapchipField* mapchipField) { mapchipField_ = mapchipField; }

private:
    void update_mapchip();
    void check_fall_conditions();
    void attach_child_to_player();
    void detach_child_from_player();
    bool approximately_equal(const Vector3& a, const Vector3& b, float epsilon = 1e-5f);
    void check_child_collision();
private:
    std::unique_ptr<CharacterBase> player;
    std::unique_ptr<CharacterBase> child;
    MapchipField* mapchipField_;

    Vector3 playerPos{};
    Vector3 childPos{};

    float deltaTime = WorldClock::DeltaSeconds();
};
