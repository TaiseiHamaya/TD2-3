#include "LineGroupResource.h"

#ifdef _DEBUG

#define VECTOR3_SERIALIZER

#include "Engine/Resources/Json/JsonSerializer.h"
#include "Engine/Resources/Json/JsonResource.h"

LineGroupResource::LineGroupResource(std::filesystem::path fileName) {
	JsonResource json{ fileName };
	std::vector<Vector3> vertices;
	std::vector<uint32_t> indexes;

	if (json.cget().contains("Vertices")) {
		const nlohmann::json& jsonVertices = json.cget().at("Vertices");
		for (const auto& elem : jsonVertices) {
			vertices.emplace_back(elem.get<Vector3>());
		}
	}

	if (json.cget().contains("Indexes")) {
		const nlohmann::json& jsonIndexes = json.cget().at("Indexes");
		for (const auto& elem : jsonIndexes) {
			indexes.emplace_back(elem.get<uint32_t>());
		}
	}

	if (!vertices.empty()) {
		vertexBuffer.write(vertices);
	}
	if (!indexes.empty()) {
		indexBuffer.write(indexes);
	}
}

LineGroupResource::LineGroupResource(std::vector<Vector3> vertices_, std::vector<uint32_t> indices) :
	vertexBuffer(vertices_),
	indexBuffer(indices) {
}

const D3D12_VERTEX_BUFFER_VIEW& LineGroupResource::get_vbv() const {
	return vertexBuffer.get_vbv();
}

const D3D12_INDEX_BUFFER_VIEW* const LineGroupResource::get_p_ibv() const {
	return indexBuffer.get_p_ibv();
}

const UINT LineGroupResource::index_size() const {
	return indexBuffer.index_size();
}

#endif // _DEBUG