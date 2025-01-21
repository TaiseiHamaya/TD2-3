#pragma once
#include <memory>

#include "Engine/Module/World/Sprite/SpriteInstance.h"
#include "Engine/Module/World/Camera/Camera3D.h"

class MeshInstance;
//class SpriteInstance;

struct AABB {
	Vector3 min;    //!< 最小点
	Vector3 max;    //!< 最大点
};

class Packet
{
public:
	void initialize();
	void finalize();
	void begin();
	void update();
	void begin_rendering();
	void late_update();
	void draw();
	void draw_sprite();

#ifdef _DEBUG
	void debug_update();
#endif // _DEBUG

public: // アクセッサ
	void set_camera(Camera3D* camera) { this->camera = camera; }

private:
	// ワールド座標をスクリーン座標に変換する関数
	Vector2 WorldToScreen(const Vector3& worldPos, const Matrix4x4& vpMatrix, const Vector2& viewportOrigin, const Vector2& viewportSize);
	Vector3 ScreenToWorld(const Vector2& screenPos, const Matrix4x4& vpMatrix, const Vector2& viewportOrigin, const Vector2& viewportSize, float depth, float nearClip, float farClip);
private:

	std::unique_ptr<MeshInstance> packet;
	AABB aabb;
	Vector3 offset{66.0f, 8.0f, 8.0f};

	Vector3 worldMouse;
	Camera3D* camera;

	float depth = 1.0f;
};