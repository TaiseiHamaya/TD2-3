#include "Application/MapchipField.h"
#include "Engine/Module/World/Mesh/MeshInstance.h"

MapchipField::MapchipField(){}

MapchipField::~MapchipField(){}

void MapchipField::init(){


	for(int i = 0; i < index; i++)
	{
		block.push_back(std::make_unique<MeshInstance>());
		block[i]->reset_mesh("RordObj.obj");
		wall.push_back(std::make_unique<MeshInstance>());
		wall[i]->reset_mesh("WallObj.obj");

	}
	goal.push_back(std::make_unique<MeshInstance>());

	loadMap(1);
}

void MapchipField::begin_rendering(){

	for(int i = 0; i < block.size(); i++)
	{
		block[i]->begin_rendering();
		wall[i]->begin_rendering();

	}
}

void MapchipField::update(){
	debugChangeStage();
}

void MapchipField::draw(){
	int blockNum = 0;//描画するインスタンスの要素番号を定義
	int wallNum = 0;
	for(int y = 0; y < culs; y++)
	{
		for(int x = 0; x < rows; x++)
		{

			if(curMap[y][x] == 1){

				block[blockNum]->get_transform()
					.set_translate(Vector3((float)x, 0, culs - (float)y));
				//block[blockNum]->get_transform().set_scale(Vector3(0.5f, 0.5f, 0.5f));
				block[blockNum]->draw();
				blockNum++;//要素番号を加算し次のインスタンスで描画出来るようにする
			} else if(curMap[y][x] == 2){
				wall[wallNum]->get_transform()
					.set_translate(Vector3((float)x, 0, culs - (float)y));
				//wall[wallNum]->get_transform().set_scale(Vector3(0.5f, 0.5f, 0.5f));
				wall[wallNum]->draw();
				wallNum++;//要素番号を加算し次のインスタンスで描画出来るようにする

			}
		}
	}

}

void MapchipField::loadMap(int stageNum){


	for(int y = 0; y < culs; y++)
	{
		for(int x = 0; x < rows; x++)
		{
			curMap[y][x] = map[stageNum][y][x];
		}
	}
}

void MapchipField::debugChangeStage(){


	if(curMapIndex < mapIndex - 1 &&
		Input::GetInstance().IsTriggerKey(KeyID::K))
	{
		curMapIndex++;
		loadMap(curMapIndex);
	}
	if(curMapIndex > 0 &&
		Input::GetInstance().IsTriggerKey(KeyID::J))
	{
		curMapIndex--;
		loadMap(curMapIndex);
	}


}
