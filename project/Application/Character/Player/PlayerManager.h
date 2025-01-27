#pragma once
#include <memory>
#include "Application/Character/Player/Player.h"
#include "Application/Character/Child/Child.h"
#include "Application/Mapchip/MapchipHandler.h"
#include "Application/Scene/GameManagement.h"


class PlayerManager {
public:
    void initialize(Reference<const LevelLoader> level, MapchipField* mapchipField);
    void finalize();
    void update();
    void begin_rendering();
    void draw() const;

#ifdef _DEBUG
    void debug_update();
#endif

    void set_game_management(GameManagement* gameManagement) { gameManagement_ = gameManagement; }
private:
    void manage_parent_child_relationship();
    void set_child_rotate();

    void attach_child_to_player(Player* player, Child* child);
    void detach_child_from_player(Player* player, Child* child);
    bool is_game_cleared() const { return gameCleared; }

private:
    std::unique_ptr<Player> player;
    std::unique_ptr<Child> child;
    std::unique_ptr<MapchipHandler> mapchipHandler;
    GameManagement* gameManagement_;
    MapchipField* mapchipField_;

    Vector3 playerPos{}; // プレイヤーの現在位置
    Vector3 childPos{};  // 子オブジェクトの現在位置

    int gameCleared = 0; // クリア状態を管理(0:通常 1:クリア 2:失敗)

};
