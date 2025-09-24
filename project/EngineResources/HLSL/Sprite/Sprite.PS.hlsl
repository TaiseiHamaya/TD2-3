#include "Sprite.hlsli"

struct Material {
	float4 color;
	uint textureIndex;
	float3x3 uvTransform;
};

StructuredBuffer<Material> gMaterial : register(t0, space0);
SamplerState gSampler : register(s0);

float4 main(VertexShaderOutput input) : SV_TARGET0 {
	Material material = gMaterial[input.instance];
	const Texture2D<float4> texture = ResourceDescriptorHeap[material.textureIndex];
	
	float4 output;
	float3 transformedUV = mul(float3(input.texcoord, 1.0f), material.uvTransform);
	float4 textureColor = texture.Sample(gSampler, transformedUV.xy);
	output = textureColor * material.color;

	return output;
}