#include "GushingParticle.h"
#include "Engine/Module/World/Mesh/MeshInstance.h"

GushingParticle::GushingParticle() {}

GushingParticle::~GushingParticle() {}

void GushingParticle::init() {

	for (int i = 0; i < objIndex; i++) {
		obj[i] = std::make_unique<particleObj>();
		obj[i]->mesh;
	}
}

void GushingParticle::update() {}

void GushingParticle::begin_rendering() {}

void GushingParticle::draw() {}

GushingParticle::particleObj::particleObj() {
	mesh = std::make_unique<MeshInstance>();
	velocity = CVector3::ZERO;
	 totalLifeTime = 0;
	 lifeTime = 0;
	 scale = 1;
	 rotateSpeed = CVector3::ZERO;
	 color = { 1.f,1.f,1.f,1.f };

}
