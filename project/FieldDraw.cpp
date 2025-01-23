#include "FieldDraw.h"
#include "Engine/Module/World/Mesh/MeshInstance.h"

FieldDraw::FieldDraw(){}

FieldDraw::~FieldDraw(){}

void FieldDraw::init(){
	block = std::make_unique<MeshInstance>();
	block->reset_mesh("RordObj.obj");

}

void FieldDraw::begin_rendering(){
	block->begin_rendering();
}

void FieldDraw::draw(){ block->draw(); }
