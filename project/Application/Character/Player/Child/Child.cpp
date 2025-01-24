#include "Child.h"

void Child::initialize()
{
	object_ = std::make_unique<MeshInstance>();
	object_->reset_mesh("ChildObj.obj");
	object_->get_transform().set_translate({ 4.0f, 1.0f, 1.0f });
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
