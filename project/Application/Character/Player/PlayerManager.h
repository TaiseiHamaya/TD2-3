#pragma once

#include <memory>

#include "./MoveLogger/MoveLogger.h"
#include "Application/Character/Child/Child.h"
#include "Application/Character/Player/Player.h"
#include "Application/Mapchip/MapchipHandler.h"
#include "Application/Scene/GameManagement.h"
#include "Engine/Resources/Audio/AudioPlayer.h"

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
	bool get_isParent() { return isParent; }
private:
	void manage_parent_child_relationship();
	void set_child_rotate();

	void attach_child_to_player(Player* player, Child* child);
	void detach_child_from_player(Player* player, Child* child);
	bool is_game_cleared() const { return stageSituation; }
	void emplace_log(const Vector3& playerPosition, const Quaternion& playerRotation);
	void undo();

private:
	std::unique_ptr<Player> player;
	std::unique_ptr<Child> child;
	std::unique_ptr<MapchipHandler> mapchipHandler;
	std::unique_ptr<MoveLogger> moveLogger;

	GameManagement* gameManagement_;
	MapchipField* mapchipField_;

	Vector3 playerPos{}; // プレイヤーの現在位置
	Vector3 childPos{};  // 子オブジェクトの現在位置

	int stageSituation = 0; // クリア状態を管理(0:通常 1:クリア 2:子が先 3:子を置いてく 4:コアラを落とす)

    bool isParent;

    //音関連
    std::unique_ptr<AudioPlayer> holdAudio;//くっつき音
    std::unique_ptr<AudioPlayer> releaseAudio;//離れる音
	
	bool isStackMovement;
};
