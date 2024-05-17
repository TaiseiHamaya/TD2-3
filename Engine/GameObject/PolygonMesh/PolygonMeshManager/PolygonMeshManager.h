#pragma once

#include <unordered_map>
#include <string>
#include <unordered_set>
#include <memory>

class PolygonMesh;

class PolygonMeshManager {
private:
	PolygonMeshManager();

public:
	~PolygonMeshManager();

private:
	PolygonMeshManager(const PolygonMeshManager&) = delete;
	PolygonMeshManager& operator=(const PolygonMeshManager&) = delete;

public:
	static PolygonMeshManager& GetInstance();
	static void LoadPolygonMesh(const std::string& directoryPath, const std::string& fileName);
	static std::weak_ptr<PolygonMesh> GetPolygonMesh(const std::string& meshName);

private:

private:
	std::unordered_map<std::string, std::shared_ptr<PolygonMesh>> meshInstanceList;
	std::unordered_set<std::string> meshRegisteredList;
};