#pragma once

#include <memory>
class MeshInstance;
class FieldDraw{
public :
	FieldDraw();
	~FieldDraw();

public:
	void Init();
	void Draw();

private:
	std::unique_ptr<MeshInstance> parent;

};

