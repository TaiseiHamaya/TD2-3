#pragma once
#include <memory>
#include "Engine/Module/World/Mesh/MeshInstance.h"

class Player
{
public:
	void load();
	void initialize();
	void finalize();

	void begin();
	void update();
	void begin_rendering();
	void late_update();

	void draw() const;

#ifdef _DEBUG
	void debug_update();
#endif // _DEBUG

public:
	Vector3 get_transform() { return object_->get_transform().get_translate(); }
	Quaternion get_rotation() { return object_->get_transform().get_quaternion(); }
	MeshInstance* get_object() { return object_.get(); }

	bool get_parent_flag() const { return isParent; }
	void set_parent_flag(bool flag) { isParent = flag; }

	bool get_move_flag() const { return isMove; }

private:
	void move();
	void rotate();

private:
	std::unique_ptr<MeshInstance> object_;
	// プレイヤーの移動速度
	float speed = 1.0f;
	// 子コアラとくっついているかどうかの判定
	bool isParent = false;
	// このフレームに移動したかどうかを取るフラグ
	bool isMove = false;

	// デバッグ用変数
	Vector3 direction{};

};