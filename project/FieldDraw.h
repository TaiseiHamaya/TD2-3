#pragma once

#include <memory>
class MeshInstance;
class FieldDraw{
public :
	FieldDraw();
	~FieldDraw();

public:
	void init();
	void begin_rendering();
	void draw();

private:
	std::unique_ptr<MeshInstance> block;

};

