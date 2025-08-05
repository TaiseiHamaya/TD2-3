#include "SpriteInstance.h"

#include <Library/Math/Transform3D.h>
#include <Library/Math/VectorConverter.h>

#include "Engine/Assets/Texture/TextureAsset.h"
#include "Engine/Assets/Texture/TextureLibrary.h"

#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
#endif // _DEBUG

SpriteInstance::SpriteInstance() : SpriteInstance("") {
}

SpriteInstance::SpriteInstance(const std::string& textureName, const Vector2& pivot_) {

	auto texture = TextureLibrary::GetTexture(textureName);
	material.textureIndex = texture->index();
	size = texture->size();
	pivot = pivot_;

#ifdef DEBUG_FEATURES_ENABLE
	material.texture = texture;
#endif // DEBUG_FEATURES_ENABLE

}

SpriteInstance::~SpriteInstance() noexcept = default;

SpriteInstance::SpriteInstance(SpriteInstance&&) noexcept = default;

SpriteInstance& SpriteInstance::operator=(SpriteInstance&&) noexcept = default;

const Transform2D& SpriteInstance::get_transform() const noexcept {
	return transform;
}

Transform2D& SpriteInstance::get_transform() noexcept {
	return transform;
}

const SpriteMaterial& SpriteInstance::get_material() const noexcept {
	return material;
}

SpriteMaterial& SpriteInstance::get_material() noexcept {
	return material;
}

Matrix4x4 SpriteInstance::create_local_matrix() const noexcept {
	return Transform3D::MakeTranslateMatrix(Converter::ToVector3(-pivot, 0)) * 
		Transform3D::MakeScaleMatrix(Converter::ToVector3(size, 1));
}

Matrix4x4 SpriteInstance::create_world_matrix() const noexcept {
	return transform.get_matrix4x4_transform();
}

//void SpriteInstance::draw() const {
//	//const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList = DxCommand::GetCommandList();
//	//// 設定したデータをコマンドに積む
//	//commandList->IASetVertexBuffers(0, 1, &vertices->get_vbv()); // VBV
//	//commandList->IASetIndexBuffer(indexes->get_p_ibv());
//	//commandList->SetGraphicsRootConstantBufferView(0, transformMatrix->get_resource()->GetGPUVirtualAddress()); // Matrix
//	//commandList->SetGraphicsRootConstantBufferView(1, material->get_resource()->GetGPUVirtualAddress()); // Color,UV
//	//commandList->SetGraphicsRootDescriptorTable(2, texture->handle()); // Texture
//	//commandList->DrawIndexedInstanced(indexes->index_size(), 1, 0, 0, 0); // 描画コマンド
//}

void SpriteInstance::set_priority(u32 priority_) noexcept {
	priority = priority_;
}

u32 SpriteInstance::key_id() const noexcept {
	return priority;
}

#ifdef DEBUG_FEATURES_ENABLE
void SpriteInstance::debug_gui() {
	transform.debug_gui();
	ImGui::Separator();
	ImGui::DragFloat2("Size", &size.x, 1.f);
	ImGui::DragFloat2("Pivot", &pivot.x, 0.01f);
	ImGui::Separator();
	ImGui::Text("Material");
	material.uvTransform.debug_gui();
	material.color.debug_gui();

	std::string textureName = material.texture->name();
	if (TextureLibrary::TextureListGui(textureName)) {
		material.textureIndex = material.texture->index();
	}
}
#endif // _DEBUG
