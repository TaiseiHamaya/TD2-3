#include "SpriteNode.h"

#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"

SpriteNode::SpriteNode() = default;

SpriteNode::~SpriteNode() noexcept = default;

void SpriteNode::initialize() {
	create_pipeline_state();
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void SpriteNode::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_VERTEX, 0);
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	rootSignatureBuilder.sampler(
		D3D12_SHADER_VISIBILITY_PIXEL,
		0, 0,
		D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR
	);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate(BlendMode::Alpha);
	psoBuilder->rasterizerstate(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "Sprite.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "Sprite.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());

}
