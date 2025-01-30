#include "Child.h"

void Child::initialize(const LevelLoader& level, MapchipHandler* mapchipHandler)
{
	object_ = std::make_unique<AnimatedMeshInstance>();
	object_->reset_animated_mesh("ChiledKoala.gltf", "Standby", true);
	object_->get_transform().set_translate(level.get_child_position());

	//音関連

	
}

void Child::finalize()
{
}

void Child::update()
{
	object_->begin();

	if (isFall) {
		Vector3 position = object_->get_transform().get_translate();
		position.y -= fallSpeed * WorldClock::DeltaSeconds();
		object_->get_transform().set_translate(position);
		if (position.y <= -3.0f) {
			isFalled = true;
		}
	
	}

	object_->update();
}

void Child::begin_rendering()
{
	object_->begin_rendering();
}

void Child::draw() const
{
	object_->draw();
}

#ifdef _DEBUG
void Child::debug_update()
{
	ImGui::Begin("Child");
	object_->debug_gui();
	ImGui::End();
}
#endif // _DEBUG