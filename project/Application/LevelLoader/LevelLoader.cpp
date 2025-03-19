#include "LevelLoader.h"

#include <format>

#include <Engine/Resources/Json/JsonResource.h>

LevelLoader::LevelLoader(int32_t level) {
	JsonResource jsonResource{
		std::format("./GameResources/Json/Level/Stage{:02}.json", level)
	};

	const nlohmann::json& root = jsonResource.cget();

	if (root.contains("player")) {
		const nlohmann::json& player = root.at("player");
		if (player.contains("row")) {
			playerRow = player.at("row").get<uint32_t>();
		}
		if (player.contains("column")) {
			playerColumn = player.at("column").get<uint32_t>();
		}
	}

	if (root.contains("child")) {
		const nlohmann::json& child = root.at("child");
		if (child.contains("row")) {
			childRow = child.at("row").get<uint32_t>();
		}
		if (child.contains("column")) {
			childColumn = child.at("column").get<uint32_t>();
		}
	}

	if (root.contains("map")) {
		const nlohmann::json& map = root.at("map");
		if (map.contains("tiles")) {
			const nlohmann::json& tiles = map.at("tiles");

			field.reserve(tiles.size());
			for (auto& array : tiles) {
				// 行要素の追加(メモリの確保)
				auto& col = field.emplace_back();
				col.reserve(array.size());

				for (auto& value : array) {
					// 値の代入
					col.emplace_back(value.get<uint32_t>());
				}
			}
		}
		if (map.contains("zerogravity")) {
			const nlohmann::json& zerogravity = map.at("zerogravity");

			isZeroGravity.reserve(zerogravity.size());
			for (auto& array : zerogravity) {
				// 行要素の追加(メモリの確保)
				auto& col = isZeroGravity.emplace_back();
				col.reserve(array.size());

				for (auto& value : array) {
					// 値の代入
					col.emplace_back(value.get<uint32_t>());
				}
			}
		}
	}
}

Vector3 LevelLoader::get_player_position() const
{
	return Vector3(static_cast<float>(playerRow), 1.0f, static_cast<float>(playerColumn));
}

Vector3 LevelLoader::get_child_position() const
{
	return Vector3(static_cast<float>(childRow), 1.0f, static_cast<float>(childColumn));
}
