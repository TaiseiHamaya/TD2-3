#pragma once

#include "Engine/Render/RenderNode/SingleRenderTargetNode.h"

#include "Engine/DirectX/DirectXResourceObject/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Math/Vector2.h"

class RadialBlurNode : public SingleRenderTargetNode {
public:
	RadialBlurNode();
	~RadialBlurNode() noexcept;

	RadialBlurNode(const RadialBlurNode&) = delete;
	RadialBlurNode& operator=(const RadialBlurNode&) = delete;
	RadialBlurNode(RadialBlurNode&&) = default;
	RadialBlurNode& operator=(RadialBlurNode&&) = default;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void initialize() override;

	/// <summary>
	/// 描画
	/// </summary>
	void draw();

	/// <summary>
	/// 描画時に使用するテクスチャリソースを設定
	/// </summary>
	/// <param name="textureGPUHandle_">テクスチャのSRVGPUハンドル</param>
	void set_texture_resource(const D3D12_GPU_DESCRIPTOR_HANDLE& textureGPUHandle_);

	void debug_gui();

private:
	/// <summary>
	/// PSO生成
	/// </summary>
	void create_pipline_state();

private:
	D3D12_GPU_DESCRIPTOR_HANDLE textureGPUHandle;

	struct BlurInfo {
		Vector2 center;
		float power;
		std::uint32_t sampleCount;
	};
	ConstantBuffer<BlurInfo> blurInfo;
};
