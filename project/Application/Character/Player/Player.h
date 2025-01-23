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
public:
	Vector3 get_transform() { return object_->get_transform().get_translate(); }
private:
	std::unique_ptr<MeshInstance> object_;

	float speed = 1.0f;
	struct PlayerData {
		
	};
};