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

private:
	std::unique_ptr<MeshInstance> object_;


};