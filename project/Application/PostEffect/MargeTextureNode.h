#pragma once

#include <Engine/Module/Render/RenderNode/SingleRenderTargetNode.h>

#include <array>

class MargeTextureNode : public SingleRenderTargetNode {
public:

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void initialize() override;

	void preprocess() override {};

	void draw() const;

	void set_texture_resources(std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 4> downSampledTextures_);

private:
	void create_pipeline_state();

private:
	std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 4> downSampledTextures;
};
