#pragma once

#include <memory>

#include "./MoveLogger/MoveLogger.h"
#include "Application/Character/Child/Child.h"
#include "Application/Character/Player/Player.h"
#include "Application/Mapchip/MapchipHandler.h"
#include "Application/GameManagement/GameManagement.h"
#include "Engine/Resources/Audio/AudioPlayer.h"

class PlayerManager {
public:
	void initialize(Reference<const LevelLoader> level, MapchipField* mapchipField, const Vector3& goalPosition);
	void finalize();
	void update();
	void handle_input();
	void begin_rendering();
	void draw() const;
	void draw_particle() const;

public:
	void restart_undo();

#ifdef _DEBUG
	void debug_update();
#endif

	void set_game_management(GameManagement* gameManagement) { gameManagement_ = gameManagement; }
	bool get_isParent() { return isParent; }
	int getStageSituation() { return stageSituation; }
private:
	void particle_update();
	void only_child_ride_update();
	void on_clear_update();

	void manage_parent_child_relationship();
	void set_child_rotate();

	void attach_child_to_player(Player* player, Child* child);
	void detach_child_from_player(Player* player, Child* child);
	void emplace_log(const Vector3& playerPosition, const Quaternion& playerRotation);
	void undo();
	bool is_game_cleared() const { return stageSituation; }

	void set_move_parameters(const Vector3& direction);
	void set_move_failed_parameters(const Vector3& direction);

	void set_rotate_parameters(const Vector3& direction);
	void set_rotate_failed_parameters(const Vector3& direction);

private:
	std::unique_ptr<Player> player;
	std::unique_ptr<Child> child;
	std::unique_ptr<MapchipHandler> mapchipHandler;
	std::unique_ptr<MoveLogger> moveLogger;
	std::unique_ptr<AnimatedMeshInstance> catchEffect_;

	// パーティクルの召還
	std::unique_ptr<ParticleEmitterInstance> dustEmitter;
	std::unique_ptr<ParticleEmitterInstance> iceDustEmitter;

	GameManagement* gameManagement_;
	MapchipField* mapchipField_;

	Vector3 playerPos{}; // プレイヤーの現在位置
	Vector3 childPos{};  // 子オブジェクトの現在位置

	int stageSituation = 0; // クリア状態を管理(0:通常 1:クリア 2:子が先 3:子を置いてく 4:コアラを落とす)

	bool isParent;

	//音関連
	std::unique_ptr<AudioPlayer> holdAudio;//くっつき音
	std::unique_ptr<AudioPlayer> releaseAudio;//離れる音
	std::unique_ptr<AudioPlayer> undoAudio;//undoの音

	bool isStackMovement;

	// 子が放すアニメーションをしているか
	bool isRerease = false;

	float clearTimer{ 0 };
	Vector3 initialPoint;
	Vector3 controlPoint;
	Vector3 terminalPoint;
};
