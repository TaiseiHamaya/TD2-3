#pragma once

#include <memory>
#include "Engine/Runtime/WorldClock/WorldClock.h"
class MeshInstance;



class GushingParticle
{
private:
	struct particleObj
	{
		std::unique_ptr<MeshInstance> mesh;
		Vector3 velocity;
		float totalLifeTime;
		float lifeTime;
		float scale;
		Vector3 rotateSpeed;
		Color3 color;
		particleObj();
		~particleObj() = default;
	};
public:
	GushingParticle();
	~GushingParticle();

	void init();
	void update();
	void begin_rendering() ;
	void draw();
private:
	static const  int objIndex = 8;

	std::array < std::unique_ptr< particleObj>, objIndex>obj;
};

