#pragma once

#include <memory>
#include <vector>

#include <Engine/Runtime/Input/Input.h>

#include <Engine/Utility/Tools/ConstructorMacro.h>
#include <Library/Math/Vector3.h>

class MeshInstance;
class WorldInstance;

template<typename T>
class Reference;

class LevelLoader;

class MapchipField {
private:
	struct Field {
		std::unique_ptr<MeshInstance> mesh;
		uint32_t type;
		bool isZeroGravity;

		Field();
		~Field() = default;
		__NON_COPYABLE_CLASS(Field)
	};

public:
	MapchipField();
	~MapchipField();

public:
	void initialize(Reference<const LevelLoader> level);
	void update();
	void begin_rendering();
	void draw();

public:
	uint32_t row() const { return rowSize; }
	uint32_t column() const { return columnSize; }
	Reference<WorldInstance> field_root() const;

public:
	//アクセッサ
	int getElement(float x, float y);
	const Vector3 &GetGoalPos() { return goalPos; }
private:

	uint32_t rowSize;
	uint32_t columnSize;

	std::unique_ptr<WorldInstance> fieldRoot;
	Vector3 goalPos;
	//メモ
	//ステージの左下が0,0右上が7,7
	std::vector<std::vector<Field>> field;
};
