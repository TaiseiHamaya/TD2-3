#pragma once

#include <memory>
#include <vector>

#include <Engine/Runtime/Input/Input.h>

class MeshInstance;

template<typename T>
class Reference;

class LevelLoader;

class MapchipField{
private:
	struct Field {
		std::unique_ptr<MeshInstance> mesh;
		uint32_t type;
		bool isZeroGravity;
	};

public:
	MapchipField();
	~MapchipField();

public:
	void initialize(Reference<const LevelLoader> level);
	void update();
	void begin_rendering();
	void draw();

	//アクセッサ
	int getElement(float x, float y);
private:

	//メモ　
	//ステージの左下が0,0右上が7,7
	 std::vector<std::vector<Field>> field;
};
