#include "SpriteDrawExecutor.h"

#include "Engine/Assets/PrimitiveGeometry/PrimitiveGeometryAsset.h"
#include "Engine/Assets/PrimitiveGeometry/PrimitiveGeometryLibrary.h"
#include "Engine/Assets/Texture/TextureAsset.h"
#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"
#include "Engine/Module/World/Sprite/SpriteInstance.h"
#include "Engine/Module/World/Camera/Camera2D.h"

#include <Library/Math/Transform2D.h>

void SpriteDrawExecutor::reinitialize(u32 maxInstance_) {
	maxInstance = maxInstance_;
	matrices.initialize(maxInstance);
	material.initialize(maxInstance);
}

void SpriteDrawExecutor::draw_command() const {
	if (instanceCounter == 0) {
		return;
	}

	auto& commandList = DxCommand::GetCommandList();
	commandList->IASetVertexBuffers(0, 0, nullptr);
	auto geometry = PrimitiveGeometryLibrary::GetPrimitiveGeometry("Rect3D");
	commandList->IASetIndexBuffer(geometry->get_p_ibv());
	commandList->SetGraphicsRootDescriptorTable(0, matrices.get_handle_gpu());
	commandList->SetGraphicsRootDescriptorTable(1, material.get_handle_gpu());
	commandList->DrawIndexedInstanced(6, instanceCounter, 0, 0, 0);
}

void SpriteDrawExecutor::write_to_buffer(Reference<const SpriteInstance> instance) {
	if(!instance || !instance->is_active()) {
		return;
	}

	u32 next;
	{
		std::lock_guard<std::mutex> lock{ writeBufferMutex };

		if (instanceCounter >= maxInstance) {
			return;
		}

		next = instanceCounter;
		++instanceCounter;
	}

	const Matrix4x4& local = instance->create_local_matrix();
	const Matrix4x4& world = instance->create_world_matrix();
	const Matrix4x4& vpMatrix = Camera2D::GetVPMatrix();
	matrices[next] = local * world * vpMatrix;
	const SpriteMaterial& source = instance->get_material();
	material[next] = {
		.color = source.color,
		.textureIndex=  0,
		.uvTransformMatrix= source.uvTransform.get_matrix(),
	};
	if (!source.textureIndex) {
		material[next].textureIndex = TextureLibrary::GetTexture("Error.png")->index();
	}
	else {
		material[next].textureIndex = source.textureIndex.value();
	}
}
