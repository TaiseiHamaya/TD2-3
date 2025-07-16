#include "Application/Mapchip/MapchipField.h"

#include <cmath>

#include "Application/LevelLoader/LevelLoader.h"

#include <Library/Utility/Template/Reference.h>

#include "Engine/Module/World/Mesh/StaticMeshInstance.h"
#include <Engine/Module/DrawExecutor/Mesh/StaticMeshDrawManager.h>

MapchipField::MapchipField() = default;

MapchipField::~MapchipField() = default;

void MapchipField::initialize(Reference<const LevelLoader> level) {
	fieldRoot = std::make_unique<WorldInstance>();

	const std::string fieldFileName[] = {
		"", // 穴
		"RordObj.obj", // 床
		"WallObj.obj", // 壁
		"RordObj.obj", // 床
		"IceObj.obj"   // 氷 
	};

	auto& fieldLevel = level->get_field();
	auto& fieldZeroGravity = level->get_zerogravity();

	constexpr float boxSize = 1.0f;

	rowSize = (uint32_t)fieldLevel.size();
	if (field.size() < rowSize) {
		field.resize(rowSize);
	}
	for (uint32_t row = 0; row < rowSize; ++row) {

		columnSize = (uint32_t)fieldLevel[row].size();
		if (field[row].size() < columnSize) {
			field[row].resize(columnSize);
		}
		for (uint32_t column = 0; column < columnSize; ++column) {
			Field& write = field[row][column];
			write.isZeroGravity = fieldZeroGravity[row][column];
			write.type = fieldLevel[row][column];
			if (write.type != 0) {
				write.mesh->set_draw(true);
				write.mesh->reset_mesh(fieldFileName[write.type]);
				auto& objMat = write.mesh->get_materials();
				for (auto& mat : objMat) {
					mat.lightingType = LighingType::None;
				}
			}
			else {
				write.mesh->set_draw(false);
			}

			write.mesh->reparent(fieldRoot, false);
			// 左下を (0, 0) とする座標変換
			write.mesh->get_transform().set_translate(
				Vector3{ column * boxSize, 0.0f, row * boxSize }
			);
			if (write.type == 3) {
				goalPos = write.mesh->get_transform().get_translate();
			}
		}
	}
}

void MapchipField::setup(Reference<StaticMeshDrawManager> manager) {
	for (auto& row : field) {
		for (auto& elem : row) {
			manager->register_instance(elem.mesh);
		}
	}
}

void MapchipField::update() {

}

void MapchipField::update_affine() {
	fieldRoot->update_affine();
	for (auto& row : field) {
		for (auto& elem : row) {
			elem.mesh->update_affine();
		}
	}
}

Reference<WorldInstance> MapchipField::field_root() const {
	return fieldRoot;
}

int MapchipField::getElement(float x, float y) {
	// 座標を整数に変換
	uint32_t ix = static_cast<uint32_t>(std::round(x));
	uint32_t iy = static_cast<uint32_t>(std::round(y));

	// 範囲チェック (0 <= ix, iy <= 6)
	if (ix < 0 || ix >= columnSize || iy < 0 || iy >= rowSize) {
		return 0; // 範囲外の場合は 0 を返す
	}

	// 範囲内の場合はマップから値を取得
	return field[iy][ix].type;
}

MapchipField::Field::Field() :
	mesh(std::make_unique<StaticMeshInstance>()),
	type(0),
	isZeroGravity(false) {
}
