#pragma once

#include <cstdint>
#include <vector>

#include <Library/Math/Vector3.h>

class LevelLoader {
public:
	LevelLoader(int32_t level);
	~LevelLoader() = default;

public:
	const std::vector<std::vector<int32_t>>& get_field() const { return field; };
	const std::vector<std::vector<bool>>& get_zerogravity() const { return isZeroGravity; };

	Vector3 get_player_position() const;
	Vector3 get_child_position() const;
private:
	uint32_t playerRow;
	uint32_t playerColumn;
	uint32_t childRow;
	uint32_t childColumn;

	std::vector<std::vector<int32_t>> field;
	std::vector<std::vector<bool>> isZeroGravity;
};
