#include "Texture.h"
#include "Texture.h"

#include "Engine/DirectX/DirectXResourceObject/DirectXResourceObject.h"
#include "Engine/DirectX/DirectXCommand/DirectXCommand.h"
#include "Engine/DirectX/DirectXDevice/DirectXDevice.h"
#include "Engine/DirectX/DirectXDescriptorHeap/SRVDescriptorHeap/SRVDescriptorHeap.h"
#include "Engine/Utility/Utility.h"
#include <d3dx12.h>

//static ;

Texture::Texture() = default;

Texture::~Texture() = default;

void Texture::set_command() const {
	assert(heapIndex.has_value());
	DirectXCommand::GetCommandList()->SetGraphicsRootDescriptorTable(2, gpuHandle); // Texture
}

Microsoft::WRL::ComPtr<ID3D12Resource> Texture::load_texture(const std::string& filePath) {
	DirectX::ScratchImage mipImages;
	mipImages = LoadTextureData(filePath); // ロード
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	create_texture_resource(metadata); // リソース生成
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = upload_texture_data(mipImages); // Texに転送

	// リソースの設定をmetadataから読み込む
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	return intermediateResource;
}

void Texture::create_resource_view() {
	// 使用するディスクリプタヒープを取得
	heapIndex = SRVDescriptorHeap::GetNextHandleIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = SRVDescriptorHeap::GetCPUHandle(heapIndex.value());
	gpuHandle = SRVDescriptorHeap::GetGPUHandle(heapIndex.value());
	// textureResourceに転送
	DirectXDevice::GetDevice()->CreateShaderResourceView(resource.Get(), &srvDesc, textureSrvHandleCPU);

}

void Texture::create_texture_resource(const DirectX::TexMetadata& metadata) {
	HRESULT hr;
	D3D12_HEAP_PROPERTIES heapProperties{};// 利用するHeapを設定
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // 通常設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // テクスチャの次元数
	resourceDesc.Width = UINT64(metadata.width); // 幅
	resourceDesc.Height = UINT(metadata.height); // 高さ
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行きor配列Textureの要素数
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); // // mipmapの数
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント
	resourceDesc.Format = metadata.format; // Textureのフォーマット

	hr = DirectXDevice::GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(resource.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<ID3D12Resource> Texture::upload_texture_data(const DirectX::ScratchImage& mipImages) {
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;
	DirectX::PrepareUpload(DirectXDevice::GetDevice().Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subResources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(resource.Get(), 0, UINT(subResources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = DirectXResourceObject::CreateBufferResource(intermediateSize);
	DirectXCommand::SetTextureCommand(resource, intermediateResource, subResources);
	return intermediateResource;
}

DirectX::ScratchImage Texture::LoadTextureData(const std::string& filePath) {
	HRESULT hr;
	DirectX::ScratchImage image{};
	auto&& filePathW = ConvertString(filePath); // Wcharに変換
	hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image); // ロード
	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImage{};
	// Mipmapに変換
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImage);
	assert(SUCCEEDED(hr));

	return mipImage;
}