#pragma once

#include "./ITextureResource.h"

#include "./IResourceView/ShaderResourceView.h"

class StaticTexture final : protected ITextureResource {
public:
	StaticTexture() = default;
	~StaticTexture();

	__CLASS_NON_COPYABLE(StaticTexture)

public:
	void initialize(Microsoft::WRL::ComPtr<ID3D12Resource> resource_, bool isCubemap = false);

	Reference<const ShaderResourceView> get_srv() const { return srv; }

	void set_name(const std::wstring& name) override;

public:
	using ITextureResource::get_width;
	using ITextureResource::get_height;

private:
	ShaderResourceView srv;
};
