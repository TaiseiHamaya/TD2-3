#pragma once

#include <Engine/Module/Render/RenderNode/SingleRenderTargetNode.h>

#include <Engine/Rendering/DirectX/DirectXResourceObject/ConstantBuffer/ConstantBuffer.h>

class BloomNode : public SingleRenderTargetNode {
public:
	BloomNode() = default;
	~BloomNode() noexcept = default;

	BloomNode(const BloomNode&) = delete;
	BloomNode& operator=(const BloomNode&) = delete;
	BloomNode(BloomNode&&) = default;
	BloomNode& operator=(BloomNode&&) = default;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void initialize() override;

	void preprocess() override {};

	void draw() const;

	void set_base_texture(D3D12_GPU_DESCRIPTOR_HANDLE baseTexture);
	void set_blur_texture(D3D12_GPU_DESCRIPTOR_HANDLE blurTexture);

	void set_param(float weight);

private:
	void create_pipeline_state();

#ifdef _DEBUG
public:
	void debug_gui();
#endif // DEBUG_FEATURES_ENABLE

private:
	D3D12_GPU_DESCRIPTOR_HANDLE baseTexture;
	D3D12_GPU_DESCRIPTOR_HANDLE blurTexture;

	struct BloomInfo {
		float weight;
	};
	ConstantBuffer<BloomInfo> bloomInfo;
};
