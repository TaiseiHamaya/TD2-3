#pragma once

#include "Application/Character/CharacterBase.h"
#include "Application/Character/Player/CharacterStates.h"
#include "Application/Mapchip/MapchipHandler.h"

#include <Engine/Assets/Audio/AudioPlayer.h>
#include <Engine/Module/World/Mesh/SkinningMeshInstance.h>
#include <Engine/Module/World/Mesh/StaticMeshInstance.h>
#include <Engine/Module/World/WorldInstance/WorldInstance.h>
#include <Engine/Runtime/Clock/WorldClock.h>

class Child : public CharacterBase {
public:
	Child();

public:
	void initialize(const LevelLoader& level, MapchipHandler* mapchipHandler) override;
	void finalize() override;
	void update() override;
	void update_affine() override;

	void set_parent(StaticMeshInstance* parent) { object_->reparent(parent);  }
	void unset_parent() { object_->reparent(nullptr); }


	ChildAnimation get_animation_info() const { return childAnimation; }
	void set_animation_info(ChildAnimation type) { childAnimation = type; }

	bool is_falled()const { return isFalled; }

	void on_undo(Vector3 position, Vector3 lookat);

#ifdef _DEBUG
	void debug_update();
#endif
private:
	bool isFall = false;
	float fallSpeed = 6.0f;//落下速度
	bool isFalled = false;
	// 
	bool isOutGround = false;

	ChildAnimation childAnimation = ChildAnimation::Normal;
	// 焦るときエフェクト
	std::unique_ptr<SkinningMeshInstance> flusteredEffect_;
	//音関連


};
