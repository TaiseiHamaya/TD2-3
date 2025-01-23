#pragma once

#include <memory>
#include <vector>
#include <Engine/Runtime/Input/Input.h>
class MeshInstance;
class MapchipField{
public:
	MapchipField();
	~MapchipField();

public:
	void init();
	void begin_rendering();
	void update();
	void draw();
	void loadMap(int stageNum);
	void debugChangeStage();


	//アクセッサ
	int getElement(int x, int y){
		return curMap[culs -y][x];
	}
private:
	std::vector < std::unique_ptr<MeshInstance>> block;
	std::vector < std::unique_ptr<MeshInstance>> wall;
	std::vector < std::unique_ptr<MeshInstance>> goal;
	const int index=49;
	static const int mapIndex = 2;
	int curMapIndex = 0;
	static const int rows = 7;//行	
	static const int culs = 7;//列


	int map[mapIndex][culs][rows] = {
		{
		{0,2,2,2,2,2,0},
		{0,1,1,1,1,1,2},
		{0,0,0,0,0,1,2},
		{0,0,0,0,1,2,0},
		{0,0,0,0,1,2,0},
		{0,0,0,1,2,0,0},
		{0,0,0,1,2,0,0},
		},
		{
		{0,2,2,2,2,2,0},
		{0,1,1,1,1,1,0},
		{0,0,0,0,0,1,0},
		{0,1,1,1,1,1,0},
		{0,1,0,0,0,0,0},
		{0,1,1,1,1,1,0},
		{0,0,0,0,0,0,0},
		}
	};

	int curMap[culs][rows];



};

