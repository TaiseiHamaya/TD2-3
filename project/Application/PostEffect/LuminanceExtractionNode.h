#pragma once

#include <Engine/Module/Render/RenderNode/SingleRenderTargetNode.h>

//#include <Engine/GraphicsAPI/DirectX/DxResource/ConstantBuffer/ConstantBuffer.h>
//#include <Engine/GraphicsAPI/DirectX/DxResource/TextureResource/RenderTexture.h>

#include <Engine/Rendering/DirectX/DirectXResourceObject/ConstantBuffer/ConstantBuffer.h>
#include <Library/Math/Color3.h>

class LuminanceExtractionNode : public SingleRenderTargetNode {
public:
	LuminanceExtractionNode() = default;
	~LuminanceExtractionNode() noexcept = default;

	LuminanceExtractionNode(const LuminanceExtractionNode&) = delete;
	LuminanceExtractionNode& operator=(const LuminanceExtractionNode&) = delete;
	LuminanceExtractionNode(LuminanceExtractionNode&&) = default;
	LuminanceExtractionNode& operator=(LuminanceExtractionNode&&) = default;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void initialize() override;

	void preprocess() override {};

	void draw() const;

	void set_texture_resource(D3D12_GPU_DESCRIPTOR_HANDLE textureGPUHandle);

	void set_param(float intensity, Color3 color);

private:
	void create_pipeline_state();

#ifdef _DEBUG
public:
	void debug_gui();
#endif // DEBUG_FEATURES_ENABLE

private:
	D3D12_GPU_DESCRIPTOR_HANDLE textureGPUHandle;

	struct LuminanceExtractionInfo {
		float intensity;
		Color3 color{ 0.2125f, 0.7154f, 0.0721f };
	};
	ConstantBuffer<LuminanceExtractionInfo> luminanceExtractionInfo;
};
