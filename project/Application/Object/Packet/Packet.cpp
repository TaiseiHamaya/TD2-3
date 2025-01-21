#include "Packet.h"
#include "Engine/Module/World/Mesh/MeshInstance.h"
#include <Engine/Application/EngineSettings.h>
#include <Engine/Module/World/Camera/Camera2D.h>
#include <Engine/Runtime/Input/Input.h>
//#include "Engine/Module/World/Sprite/SpriteInstance.h"

void Packet::initialize()
{
	packet = std::make_unique<MeshInstance>();
	packet->reset_object("cube.obj");
	packet->get_transform().set_scale({ 64.0f, 8.0f, 8.0f });
	packet->get_transform().set_translate_z(430.0f);

}

void Packet::finalize()
{
}

void Packet::begin()
{
}

void Packet::update()
{
	// マウスの位置の計算
	Vector2 screenPos = Input::GetInstance().MousePosition(); // スクリーン座標 (中央)
	Matrix4x4 vpMatrix = camera->vp_matrix(); // ビュー・プロジェクション行列
	Vector2 viewportOrigin = { 0.0f, 0.0f }; // ビューポートの原点
	Vector2 viewportSize = EngineSettings::CLIENT_SIZE; // スクリーンサイズ
	// 深度値 (0:近いクリップ面, 1:遠いクリップ面)
	float nearClip = 0.1f;
	float farClip = 500.0f;

	Vector3 worldPos = ScreenToWorld(screenPos, vpMatrix, viewportOrigin, viewportSize, depth, nearClip, farClip);
	worldMouse = worldPos;

	ImGui::Begin("aabb");
	ImGui::DragFloat3("min", &offset.x, 0.01f);
	ImGui::End();

	aabb.min = packet->get_transform().get_translate() - offset;
	aabb.max = packet->get_transform().get_translate() + offset;

	if (aabb.min.x < worldMouse.x && aabb.max.x > worldMouse.x &&
		aabb.min.y < worldMouse.y && aabb.max.y > worldMouse.y &&
		aabb.min.z < worldMouse.z && aabb.max.z > worldMouse.z) {
		if (Input::GetInstance().IsPressMouse(MouseID::Left)) {
			packet->set_active(false);
		}
		else {
			packet->set_active(true);
		}
	}
	else {
		packet->set_active(true);

	}
}

void Packet::begin_rendering()
{
	packet->begin_rendering();
}

void Packet::late_update()
{
}

void Packet::draw()
{
	packet->draw();
}

void Packet::draw_sprite()
{
}

#ifdef _DEBUG
void Packet::debug_update()
{
	ImGui::Begin("Packet");
	packet->debug_gui();
	ImGui::End();

}
#endif // _DEBUG

Vector2 Packet::WorldToScreen(const Vector3& worldPos, const Matrix4x4& vpMatrix, const Vector2& viewportOrigin, const Vector2& viewportSize)
{
	// 1. ワールド座標をビュー・プロジェクション行列で変換 (w = 1 を仮定)
	float clipX = vpMatrix[0][0] * worldPos.x + vpMatrix[1][0] * worldPos.y + vpMatrix[2][0] * worldPos.z + vpMatrix[3][0];
	float clipY = vpMatrix[0][1] * worldPos.x + vpMatrix[1][1] * worldPos.y + vpMatrix[2][1] * worldPos.z + vpMatrix[3][1];
	float clipZ = vpMatrix[0][2] * worldPos.x + vpMatrix[1][2] * worldPos.y + vpMatrix[2][2] * worldPos.z + vpMatrix[3][2];
	float clipW = vpMatrix[0][3] * worldPos.x + vpMatrix[1][3] * worldPos.y + vpMatrix[2][3] * worldPos.z + vpMatrix[3][3];

	// 2. 正規化デバイス座標 (NDC) に変換 (w で割る)
	if (clipW != 0.0f) {
		clipX /= clipW;
		clipY /= clipW;
		clipZ /= clipW;
	}

	// 3. スクリーン座標に変換
	Vector2 screenPos;
	screenPos.x = (clipX + 1.0f) * 0.5f * viewportSize.x + viewportOrigin.x;
	screenPos.y = (1.0f - clipY) * 0.5f * viewportSize.y + viewportOrigin.y;

	return screenPos;
}

Vector3 Packet::ScreenToWorld(const Vector2& screenPos, const Matrix4x4& vpMatrix, const Vector2& viewportOrigin, const Vector2& viewportSize, float depth, float nearClip, float farClip)
{
	// 1. スクリーン座標を正規化デバイス座標 (NDC) に変換
	Vector3 ndcPos;
	ndcPos.x = (2.0f * (screenPos.x - viewportOrigin.x) / viewportSize.x) - 1.0f;
	ndcPos.y = 1.0f - (2.0f * (screenPos.y - viewportOrigin.y) / viewportSize.y);

	// 非線形の深度を線形に戻す
	float linearDepth = nearClip * farClip / (farClip - depth * (farClip - nearClip));
	ndcPos.z = (linearDepth - nearClip) / (farClip - nearClip) * 2.0f - 1.0f;

	// 2. ビュー・プロジェクション行列の逆行列を取得
	Matrix4x4 invVPMatrix = Matrix4x4::Inverse(vpMatrix);

	// 3. 正規化デバイス座標をワールド空間に変換
	float w = invVPMatrix[0][3] * ndcPos.x + invVPMatrix[1][3] * ndcPos.y + invVPMatrix[2][3] * ndcPos.z + invVPMatrix[3][3];
	Vector3 worldPos;
	worldPos.x = (invVPMatrix[0][0] * ndcPos.x + invVPMatrix[1][0] * ndcPos.y + invVPMatrix[2][0] * ndcPos.z + invVPMatrix[3][0]) / w;
	worldPos.y = (invVPMatrix[0][1] * ndcPos.x + invVPMatrix[1][1] * ndcPos.y + invVPMatrix[2][1] * ndcPos.z + invVPMatrix[3][1]) / w;
	worldPos.z = (invVPMatrix[0][2] * ndcPos.x + invVPMatrix[1][2] * ndcPos.y + invVPMatrix[2][2] * ndcPos.z + invVPMatrix[3][2]) / w;

	return worldPos;
}

