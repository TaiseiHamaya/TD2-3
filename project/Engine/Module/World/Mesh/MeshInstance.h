#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Library/Math/Transform2D.h"

#include "Engine/Module/World/WorldInstance/WorldInstance.h"

class Material;
class PolygonMesh;
class Texture;
class TransformMatrix;
class Color3;

class MeshInstance : public WorldInstance {
	friend class AnimatedMeshInstance;
public:
	struct PolygonMeshMaterial {
		PolygonMeshMaterial();
		friend class MeshInstance;
		friend class AnimatedMeshInstance;
	private:
		std::shared_ptr<Texture> texture;
		std::unique_ptr<Material> material;

	public:
		Color3& color;
		Transform2D uvTransform;

#ifdef _DEBUG
	private:
		std::string textureName;
#endif // _DEBUG
	};

public:
	MeshInstance() noexcept(false);
	explicit MeshInstance(const std::string& meshName_) noexcept(false);
	virtual ~MeshInstance() noexcept;

	MeshInstance(MeshInstance&&) noexcept;
	MeshInstance& operator=(MeshInstance&&) noexcept;

private:
	MeshInstance(const MeshInstance&) = delete;
	MeshInstance& operator=(const MeshInstance&) = delete;

public:
	virtual void begin() {};
	virtual void update() {};
	virtual void begin_rendering() noexcept;
	virtual void late_update() {};
	virtual void draw() const;

	void reset_object(const std::string& meshName_);

	/// <summary>
	/// Texture、Materialパラメータ、UVデータのリセットを行う
	/// </summary>
	void default_material();

protected:
	struct MaterialDataRef;

public:
	std::vector<PolygonMeshMaterial>& get_materials();

protected:
	void set_texture(const std::string& name, int index = 0);

#ifdef _DEBUG
public:
	void debug_gui() override;
#endif // _DEBUG

protected:
	bool isDraw = true;

private:
	std::shared_ptr<PolygonMesh> mesh;

	std::unique_ptr<TransformMatrix> transformMatrix;
	
	std::vector<PolygonMeshMaterial> meshMaterials;

#ifdef _DEBUG
private:
	std::string meshName;
#endif // _DEBUG
};
