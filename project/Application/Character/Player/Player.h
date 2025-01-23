#pragma once
#include <memory>
#include "Engine/Module/World/Mesh/MeshInstance.h"
#include <Application/MapchipField.h>

class Player
{
public:
	void initialize();
	void finalize();

	void begin();
	void update(MapchipField* mapchipField);
	void begin_rendering();
	void late_update();

	void draw() const;

#ifdef _DEBUG
	void debug_update();
#endif // _DEBUG

public:
	Vector3 get_transform() { return object_->get_transform().get_translate(); }
	//Vector3 get_world_pos(){return object_->get_transform(). }

	Quaternion get_rotation() { return object_->get_transform().get_quaternion(); }
	MeshInstance* get_object() { return object_.get(); }

	bool get_parent_flag() const { return isParent; }
	void set_parent_flag(bool flag) { isParent = flag; }

	void set_fall_flag(bool flag) { isFall = flag; }

	bool get_move_flag() const { return isMove; }

private:
	void move(MapchipField* mapchipField);
	void rotate();

private:
	std::unique_ptr<MeshInstance> object_;
	// プレイヤーの移動速度
	float speed = 1.0f;
	// 子コアラとくっついているかどうかの判定
	bool isParent = false;
	// このフレームに移動したかどうかを取るフラグ
	bool isMove = false;
	// プレイヤーが落下中かどうかを判定するフラグ
	bool isFall = false;
	// プレイヤーの向きを取る変数
	Vector3 direction{};
};