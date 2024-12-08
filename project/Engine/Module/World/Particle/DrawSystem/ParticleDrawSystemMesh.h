#pragma once

#include "BaseParticleDrawSystem.h"
#include "Engine/Resources/PolygonMesh/PolygonMesh.h"

#include <string>

class ParticleDrawSystemMesh final : public BaseParticleDrawSystem {
public:
	ParticleDrawSystemMesh(const std::string& meshName);
	~ParticleDrawSystemMesh() = default;

public:
	void draw_command(size_t InstanceCount) const override;

	void set_mesh(const std::string& meshName);

private:
	std::weak_ptr<PolygonMesh> mesh;
	std::weak_ptr<Texture> texture;
};
