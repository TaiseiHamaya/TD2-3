#include "FullscreenShader.hlsli"

struct ExtractionInfo {
	float luminanceIntensity;
	float3 color;
};

ConstantBuffer<ExtractionInfo> gExtractionInfo : register(b0);
Texture2D<float4> gTexture : register(t0);

float4 main(VertexShaderOutput input) : SV_TARGET0 {
	float4 textureColor = gTexture.Load(int3(input.position.xy, 0));
	float luminance = dot(textureColor.xyz, gExtractionInfo.color);
	
	clip(luminance - gExtractionInfo.luminanceIntensity);
	
	return textureColor;
}