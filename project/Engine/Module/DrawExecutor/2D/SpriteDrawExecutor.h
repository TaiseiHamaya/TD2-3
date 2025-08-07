#pragma once

#include <mutex>

#include "../BaseDrawExecutor.h"

#include <Library/Math/Matrix4x4.h>
#include <Library/Utility/Template/Reference.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/StructuredBuffer/StructuredBuffer.h"

class SpriteInstance;

class SpriteDrawExecutor final : public BaseDrawExecutor<SpriteInstance> {
public:
	SpriteDrawExecutor() noexcept = default;
	~SpriteDrawExecutor() noexcept = default;

	__CLASS_NON_COPYABLE(SpriteDrawExecutor)

public:
	void reinitialize(u32 maxInstance_);
	void draw_command() const override;
	void write_to_buffer(Reference<const SpriteInstance> instance) override;

private:
	StructuredBuffer<Matrix4x4> matrices;
	StructuredBuffer<SpriteMaterialDataBuffer> material;

	std::mutex writeBufferMutex;
};
