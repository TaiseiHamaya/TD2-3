#include "TextureManager.h"

#include <mutex>
#include <ranges>

#include "Engine/Resources/BackgroundLoader/BackgroundLoader.h"
#include "Engine/Rendering/DirectX/DirectXResourceObject/Texture/Texture.h"
#include "Engine/Debug/Output.h"

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

std::mutex textureMutex;

TextureManager::TextureManager() = default;

TextureManager::~TextureManager() noexcept = default;

TextureManager& TextureManager::GetInstance() noexcept {
	static TextureManager instance{};
	return instance;
}

void TextureManager::Initialize() noexcept {
	GetInstance();
}

void TextureManager::Finalize() {
	auto&& list = GetInstance().textureInstanceList;
	for (auto& texture : list | std::views::values) {
		texture->release_srv_heap();
	}
}

void TextureManager::RegisterLoadQue(const std::filesystem::path& filePath) {
	if (IsRegistered(filePath.filename().string())) {
		return;
	}
	BackgroundLoader::RegisterLoadQue(LoadEvent::LoadTexture, filePath);
}

std::shared_ptr<Texture> TextureManager::GetTexture(const std::string& textureName) noexcept(false) {
	std::lock_guard<std::mutex> lock{ textureMutex };
	// 見つかったらそのデータのweak_ptrを返す
	if (IsRegisteredNonlocking(textureName)) {
		return GetInstance().textureInstanceList.at(textureName);
	}
	else {
		Console("[TextureManager] Texture Name-\'{:}\' is not loading.\n", textureName);
		return GetInstance().textureInstanceList.at("Error.png");
	}
}

bool TextureManager::IsRegistered(const std::string& textureName) noexcept(false) {
	std::lock_guard<std::mutex> lock{ textureMutex };
	return IsRegisteredNonlocking(textureName);
}

void TextureManager::UnloadTexture(const std::string& textureName) {
	std::lock_guard<std::mutex> lock{ textureMutex };
	if (IsRegisteredNonlocking(textureName)) {
		Console("[TextureManager] Unload texture Name-\'{:}\'.\n", textureName);
		auto&& texture = GetInstance().textureInstanceList.at(textureName);
		texture->release_srv_heap();
		texture.reset();
		GetInstance().textureInstanceList.erase(textureName);
	}
}

void TextureManager::Transfer(const std::string& name, std::shared_ptr<Texture>& data) {
	std::lock_guard<std::mutex> lock{ textureMutex };
	if (IsRegisteredNonlocking(name)) {
		data->release_srv_heap();
		Console("[TextureManager] Transferring registered texture. Name-\'{:}\', Address-\'{:}\'\n", name, (void*)data.get());
		return;
	}
	Console("[TextureManager] Transfer new Texture. Name-\'{:}\', Address-\'{:}\'\n", name, (void*)data.get());
	GetInstance().textureInstanceList.emplace(name, data);
}

#ifdef _DEBUG
bool TextureManager::TextureListGui(std::string& current) {
	bool changed = false;

	std::lock_guard<std::mutex> lock{ textureMutex };
	if (ImGui::BeginCombo("TextureList", current.c_str())) {
		auto&& list = GetInstance().textureInstanceList;
		for (const auto& name : list | std::views::keys) {
			bool is_selected = (current == name);
			if (ImGui::Selectable(name.c_str(), is_selected)) {
				current = name;
				changed = true;
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();

	}
	return changed;
}
#endif // _DEBUG

bool TextureManager::IsRegisteredNonlocking(const std::string& textureName) noexcept(false) {
	return GetInstance().textureInstanceList.contains(textureName);
}
