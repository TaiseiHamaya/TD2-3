#pragma once

#include <cstdint>
#include <vector>

class LevelLoader {
public:
	LevelLoader(uint32_t level);
	~LevelLoader() = default;

public:
	const std::vector<std::vector<int32_t>>& get_field() const { return field; };
	const std::vector<std::vector<bool>>& get_zerogravity() const { return isZeroGravity; };

private:
	uint32_t playerRow;
	uint32_t playerColumn;
	uint32_t childRow;
	uint32_t childColumn;

	std::vector<std::vector<int32_t>> field;
	std::vector<std::vector<bool>> isZeroGravity;
};
