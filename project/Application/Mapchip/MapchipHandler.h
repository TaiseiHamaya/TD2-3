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

    void check_fall_conditions(Player* player, Child* child);
    int is_goal_reached(Player* player, Child* child) const;
    
private:

    MapchipField* mapchipField_;

};