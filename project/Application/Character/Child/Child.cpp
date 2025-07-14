#include "Child.h"

void Child::initialize(const LevelLoader& level, MapchipHandler* mapchipHandler)
{
	object_ = std::make_unique<SkinningMeshInstance>();
	object_->reset_animated_mesh("ChiledKoala.gltf", "Standby", true);
	object_->get_transform().set_translate(level.get_child_position());

	flusteredEffect_ = std::make_unique<SkinningMeshInstance>();
	flusteredEffect_->reset_animated_mesh("FlusteredEffect.gltf", "Standby", true);
	flusteredEffect_->set_active(false);

	auto& objMat = object_->get_materials();
	for (auto& mat : objMat) {
		mat.lightingType = LighingType::None;
	}
	//音関連

	
}

void Child::finalize()
{
}

void Child::update()
{
	object_->begin();
	flusteredEffect_->begin();

	if (!isFalling) {
		isFalled = false;
	}

	if (isFalling) {
		Vector3 position = object_->get_transform().get_translate();
		position.y -= fallSpeed * WorldClock::DeltaSeconds();
		object_->get_transform().set_translate(position);
		if (position.y <= -3.0f) {
			isFalled = true;
		}
	
	}

	// 焦る時のエフェクトを子供の上に置いておく
	Vector3 flusteredPos = object_->world_position();
	flusteredPos.y += 1.0f;
	flusteredEffect_->get_transform().set_translate(flusteredPos);
	if (childAnimation == ChildAnimation::Flustered) {
		flusteredEffect_->set_active(true);
	}
	else {
		flusteredEffect_->set_active(false);
	}
	flusteredEffect_->update();

	object_->update();
}

void Child::update_affine() {
	object_->update_affine();
	flusteredEffect_->update_affine();
}

void Child::on_undo(Vector3 position, Vector3 lookat) {
	object_->get_transform().set_translate(position);
	object_->look_at(lookat);
}

#ifdef _DEBUG
void Child::debug_update()
{
	//ImGui::Begin("Child");
	//object_->debug_gui();
	//ImGui::End();
}
#endif // _DEBUG