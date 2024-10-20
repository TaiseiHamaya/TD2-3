#include "BaseParticleSystem.h"

#include <optional>

#include "Particle/Movements/BaseParticleMovements.h"

BaseParticleSystem::BaseParticleSystem() = default;

BaseParticleSystem::~BaseParticleSystem() = default;

void BaseParticleSystem::initialize(uint32_t numMaxParticle_) {
	numMaxParticle = numMaxParticle_;
	create_buffers();
}

void BaseParticleSystem::finalize() {
	particleBuffer.finalize();
}

void BaseParticleSystem::update() {
	if (!emitter) {
		return;
	}
	// エミッター更新
	if (emitter->is_active()) {
		emitter->update();
	}
	// パティクルの更新
	for (Particle& particle : particles) {
		if (particle.is_active()) {
			particle.update();
		}
	}
	// 削除
	particles.remove_if(
		[&](Particle& particle) {
		if (particle.is_destroy()) {
			particleBuffer.get_array()[particle.used_index()].isDraw = false;
			releasedIndex.emplace_back(particle.used_index());
			return true;
		}
		return false;
	});
	// 生成
	if (emitter->is_active() && emitter->is_emit()) {
		uint32_t numEmits = emitter->num_emits();
		if (numEmits && !emitter->is_end()) {
			for (uint32_t i = 0; i < numEmits; ++i) {
				emit();
			}
		}
	}
}

void BaseParticleSystem::begin_rendering() {
	for (Particle& particle : particles) {
		particle.begin_rendering();
	}
}

void BaseParticleSystem::emit() {
	std::optional<uint32_t> useIndex;
	if (!releasedIndex.empty()) {
		useIndex = releasedIndex.front();
		releasedIndex.pop_front();
	}
	else if (nextUseIndex < numMaxParticle) {
		useIndex = nextUseIndex;
		++nextUseIndex;
	}
	if (useIndex.has_value()) {
		particleBuffer.get_array()[useIndex.value()].isDraw = true;
		particles.emplace_back(
			useIndex.value(),
			particleBuffer.get_array()[useIndex.value()],
			particleMovements ? particleMovements->clone() : nullptr
		);
	}
}

void BaseParticleSystem::set_emitter(std::unique_ptr<BaseEmitter>&& emitter_) {
	emitter = std::move(emitter_);
	emitter->initialize();
}

void BaseParticleSystem::set_particle_movements(std::unique_ptr<BaseParticleMovements>&& particleMovements_) {
	particleMovements = std::move(particleMovements_);
}

void BaseParticleSystem::create_buffers() {
	particleBuffer.initialize(numMaxParticle);
}
