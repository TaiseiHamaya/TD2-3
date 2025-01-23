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
	int blockNum = 0;//描画するインスタンスの要素番号を定義
	for(int y = 0; y < 7; y++)
	{
		for(int x = 0; x < 7; x++)
		{

			if(map[y][x] == 1){

				block[blockNum]->get_transform()
					.set_translate(Vector3((float)x, 0, 7-(float)y));
				block[blockNum]->get_transform().set_scale(Vector3(0.5f, 0.5f, 0.5f));
				block[blockNum]->draw();
				blockNum++;//要素番号を加算し次のインスタンスで描画出来るようにする
			}
		}
	}
	
}
