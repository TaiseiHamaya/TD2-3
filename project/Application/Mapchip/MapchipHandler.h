#pragma once
#include "Application/Mapchip/MapchipField.h"
#include "Library/Math/Vector3.h"
#include "Library/Math/Quaternion.h"

class Player;
class Child;

class MapchipHandler {
public:
    void initialize(MapchipField* mapchipField);

    void update_player_on_mapchip(Player* player, Child* child);

    bool can_player_move_to(Player* player, Child* child, const Vector3& position) const;
    bool can_player_move_on_ice(Player* player, Child* child, const Vector3& position) const;
    bool player_move_to_wall_or_holl(Player* player, Child* child, const Vector3& position) const;

    bool can_player_rotate(Player* player, Child* child, const Vector3& direction) const;
    void setup_rotation_parameters(Player* player, Child* child, const Vector3& direction);
    void check_fall_conditions(Player* player, Child* child);
    int is_goal_reached(Player* player, Child* child) const;
    
private:

    MapchipField* mapchipField_;

};