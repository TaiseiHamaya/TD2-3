#include "Child.h"

void Child::load()
{
}

void Child::initialize()
{
	object_ = std::make_unique<MeshInstance>();
	object_->reset_mesh("ChildObj.obj");
	object_->get_transform().set_translate({ 4.0f, 1.0f, 2.0f });
}

void Child::finalize()
{
}

void Child::begin()
{
}

void Child::update()
{
}

void Child::begin_rendering()
{
	object_->begin_rendering();
}

void Child::late_update()
{
}

void Child::draw() const
{
	object_->draw();
}
