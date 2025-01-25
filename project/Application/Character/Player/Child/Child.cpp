#include "Child.h"

void Child::initialize(const LevelLoader& level)
{
	object_ = std::make_unique<MeshInstance>();
	object_->reset_mesh("ChiledKoala.obj");
	object_->get_transform().set_translate(level.get_child_position());
}

void Child::finalize()
{
}

void Child::update()
{
	if (isFall) {
		Vector3 position = object_->get_transform().get_translate();
		position.y -= 0.1f;
		object_->get_transform().set_translate(position);
	}
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