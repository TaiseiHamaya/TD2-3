#include "Sprite.hlsli"

#include "Tools/MathTool.hlsli"

static const float2 Positions[] = {
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
};

static const float2 Texcoords[] = {
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
};

StructuredBuffer<float4x4> gTransformMatrix : register(t0, space0);

VertexShaderOutput main(uint index : SV_VertexID, uint instance : SV_InstanceID) {
	VertexShaderOutput output;
	
	output.position = mul(float4(Positions[index], 0.0f, 1.0f), gTransformMatrix[instance]);
	output.texcoord = Texcoords[index];
	output.instance = instance;
	return output;
}