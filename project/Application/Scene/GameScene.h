#pragma once
#include <memory>
#include "Engine/Runtime/Scene/BaseScene.h"
#include "Application/Object/Packet/Packet.h"
#include "Engine/Module/World/Camera/Camera3D.h"
#include "Engine/Resources/Json/JsonResource.h"
#include "Engine/Module/World/Light/DirectionalLight/DirectionalLightInstance.h"

class RenderPath;
class Camera3D;

class GameScene : public BaseScene
{
public:
	GameScene();
	~GameScene();

public:
	void load() override;
	void initialize() override;
	void popped() override;
	void finalize() override;

	void begin() override;
	void update() override;
	void begin_rendering() override;
	void late_update() override;

	void draw() const override;

#ifdef _DEBUG
	void debug_update() override;
#endif // _DEBUG
private:
	std::unique_ptr<RenderPath> renderPath;

	std::unique_ptr<DirectionalLightInstance> directionalLight;
	std::unique_ptr<Camera3D> camera3D;
	std::unique_ptr<Packet> packet;

};

