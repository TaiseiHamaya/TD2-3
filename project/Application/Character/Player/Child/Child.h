#pragma once
#include <memory>
#include "Engine/Module/World/Mesh/MeshInstance.h"

class Child
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

public:
	// 平行移動を取得
	Vector3 get_translate() { return object_->get_transform().get_translate(); }

	void set_translate(Vector3 translate) { object_->get_transform().set_translate(translate); }
	// 親子付けする
	void set_parent(MeshInstance* parent) { object_->reparent(parent); }

private:
	std::unique_ptr<MeshInstance> object_;

	//float speed = 1.0f;

};

