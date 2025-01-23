#include "FieldDraw.h"
#include "Engine/Module/World/Mesh/MeshInstance.h"

FieldDraw::FieldDraw(){}

FieldDraw::~FieldDraw(){}

void FieldDraw::init(){


	for(int i = 0; i < index; i++)
	{
		block.push_back(std::make_unique<MeshInstance>());
		//bloc = std::make_unique<MeshInstance>();
		block[i]->reset_mesh("RordObj.obj");
	}
	

}

void FieldDraw::begin_rendering(){

	for(int i = 0; i < block.size(); i++)
	{
		block[i]->begin_rendering();

	}
}

void FieldDraw::draw(){ 
	for(int i = 0; i < block.size(); i++)
	{
		block[i]->get_transform().set_translate_x((float)i*2);
		block[i]->draw();
	}
	
	
	 }
