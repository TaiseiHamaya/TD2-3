#include "Application/Mapchip/MapchipField.h"
#include "Engine/Module/World/Mesh/MeshInstance.h"
#include "Application/LevelLoader/LevelLoader.h"

MapchipField::MapchipField() = default;

MapchipField::~MapchipField() = default;

void MapchipField::initialize(const LevelLoader& level) {
	const std::string fieldFileName[] = {
		"", // 穴
		"RordObj.obj", // 床
		"WallObj.obj", // 壁
		"GoalObj.obj", // ゴール
		"IceObj.obj"   // 氷 
	};

	auto& fieldLevel = level.get_field();
	auto& fieldZeroGravity = level.get_zerogravity();


	constexpr float boxSize = 1.0f;
	field.resize(fieldLevel.size());
	for (uint32_t row = 0; row < fieldLevel.size(); ++row) {
		field[row].resize(fieldLevel[row].size());
		for (uint32_t column = 0; column < fieldLevel[row].size(); ++column) {
			field[row][column].isZeroGravity = fieldZeroGravity[row][column];
			field[row][column].type = fieldLevel[row][column];
			if (field[row][column].type != 0) {
				field[row][column].mesh = std::make_unique<MeshInstance>(fieldFileName[field[row][column].type]);
			}
			else {
				field[row][column].mesh = std::make_unique<MeshInstance>();
			}

			// 左下を (0, 0) とする座標変換
			field[row][column].mesh->get_transform().set_translate(
				Vector3{ column * boxSize, 0.0f, row * boxSize }
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

int MapchipField::getElement(float x, float y)
{
	// 座標を整数に変換
	int ix = static_cast<int>(x);
	int iy = static_cast<int>(y);

	// 範囲チェック (0 <= ix, iy <= 6)
	if (ix < 0 || ix > 6 || iy < 0 || iy > 6) {
		return 0; // 範囲外の場合は 0 を返す
	}

	// 範囲内の場合はマップから値を取得
	return field[iy][ix].type;
}
