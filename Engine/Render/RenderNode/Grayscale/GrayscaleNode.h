#pragma once

#include "Engine/Render/RenderNode/SingleRenderTargetNode.h"

#include "Engine/DirectX/DirectXResourceObject/ConstantBuffer/ConstantBuffer.h"

class GrayscaleNode : public SingleRenderTargetNode {
public:
	GrayscaleNode();
	~GrayscaleNode();

	GrayscaleNode(const GrayscaleNode&) = delete;
	GrayscaleNode& operator=(const GrayscaleNode&) = delete;
	GrayscaleNode(GrayscaleNode&&) = default;
	GrayscaleNode& operator=(GrayscaleNode&&) = default;

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

#ifdef _DEBUG
	void debug_gui();
#endif // _DEBUG
		
private:
	/// <summary>
	/// PSO生成
	/// </summary>
	void create_pipline_state();

private:
	D3D12_GPU_DESCRIPTOR_HANDLE textureGPUHandle{};
	ConstantBuffer<std::uint32_t> isGray{};
};
