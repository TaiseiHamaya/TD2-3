#include "Application/MapchipField.h"
#include "Engine/Module/World/Mesh/MeshInstance.h"

#include "Application/LevelLoader/LevelLoader.h"

MapchipField::MapchipField() = default;

MapchipField::~MapchipField() = default;

void MapchipField::initialize(const LevelLoader& level) {
	const std::string fieldFileName[] = {
		"", // 穴
		"RordObj.obj", // 床
		"WallObj.obj", // 壁
		"GoalObj.obj" // ゴール
	};

	auto& fieldLevel = level.get_field();
	auto& fieldZeroGravity = level.get_zerogravity();


	constexpr float boxSize = 1.0f;
	field.resize(fieldLevel.size());
	for (uint32_t row = 0; row < fieldLevel.size(); ++row) {
		field[row].resize(fieldLevel[row].size());
		for (uint32_t column = 0; column < fieldLevel.size(); ++column) {
			field[row][column].isZeroGravity = fieldZeroGravity[row][column];
			field[row][column].type = fieldLevel[row][column];
			if (field[row][column].type != 0) {
				field[row][column].mesh = std::make_unique<MeshInstance>(fieldFileName[field[row][column].type]);
			}
			else {
				field[row][column].mesh = std::make_unique<MeshInstance>();
			}
			field[row][column].mesh->get_transform().set_translate(
				Vector3{ row * 1.0f , 0.0f, column * 1.0f }
			);
		}
	}
}

void MapchipField::update() {
}

void MapchipField::begin_rendering() {
	for (auto& vec : field) {
		for (auto& elem : vec) {
			elem.mesh->begin_rendering();
		}
	}
}

void MapchipField::draw() {
	for (auto& vec : field) {
		for (auto& elem : vec) {
			elem.mesh->draw();
		}
	}
}
