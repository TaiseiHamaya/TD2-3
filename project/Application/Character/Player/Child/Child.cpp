#include "Child.h"

void Child::load()
{
}

void Child::initialize()
{
	object_ = std::make_unique<MeshInstance>();
	object_->reset_mesh("ChildObj.obj");
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
