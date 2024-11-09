#include "ParticleBillboardNode.h"

#include "Engine/Rendering/DirectX/PipelineState/PipelineState.h"
#include "Engine/Rendering/DirectX/PipelineState/PSOBuilder/PSOBuilder.h"
#include "Engine/Rendering/DirectX/DirectXResourceObject/DepthStencil/DepthStencil.h"

ParticleBillboardNode::ParticleBillboardNode() = default;

ParticleBillboardNode::~ParticleBillboardNode() noexcept = default;

void ParticleBillboardNode::initialize() {
	depthStencil = DepthStencilValue::depthStencil;
	create_pipeline_state();
	pipelineState->set_name("ParticleBillboardNode");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void ParticleBillboardNode::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_VERTEX, 0, 1); // 0 :  transform
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_VERTEX, 0); // 1 : camera
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 0); // 2 : texture
	rootSignatureBuilder.sampler( // sampler
		D3D12_SHADER_VISIBILITY_PIXEL,
		0,
		D3D12_FILTER_ANISOTROPIC
	);

	InputLayoutBuilder inputLayoutBuilder;
	inputLayoutBuilder.add_element("POSITION", 0, DXGI_FORMAT_R32G32_FLOAT);
	inputLayoutBuilder.add_element("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	ShaderBuilder shaderManager;
	shaderManager.initialize(
		"EngineResources/HLSL/Particle/ParticleBillboard/ParticleBillboard.VS.hlsl",
		"EngineResources/HLSL/Particle/ParticleBillboard/ParticleBillboard.PS.hlsl"
	);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate();
	psoBuilder->depthstencilstate(*depthStencil);
	psoBuilder->inputlayout(inputLayoutBuilder.build());
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(shaderManager);
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<PipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());

}
