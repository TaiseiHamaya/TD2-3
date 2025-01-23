#pragma once

#include <memory>
#include <vector>
class MeshInstance;
class FieldDraw{
public:
	FieldDraw();
	~FieldDraw();

public:
	void init();
	void begin_rendering();
	void draw();

private:
	std::vector < std::unique_ptr<MeshInstance>> block;
	const int index=10;
};

