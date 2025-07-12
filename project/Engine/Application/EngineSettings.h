#pragma once

#include <filesystem>
#include <format>
#include <cstdint>
#include <string_view>

#include <Library/Math/Vector2.h>
#include <Engine/Utility/Tools/ChronoUtility.h>

namespace EngineSettings {
	// Windowタイトル
	// Framework作ったらそっちに移行する？
	static constexpr std::wstring_view WINDOW_TITLE_W{ L"2306_コスモアラ" };

	// DefaultWindowSetting
	//static constexpr DWORD WindowConfig

	// WindowSize
	static constexpr Vector2 CLIENT_SIZE{ 1280, 720 };
	static constexpr std::uint32_t  CLIENT_WIDTH{ static_cast<std::uint32_t>(CLIENT_SIZE.x) };
	static constexpr std::uint32_t  CLIENT_HEIGHT{ static_cast<std::uint32_t>(CLIENT_SIZE.y) };
	
	// ErrorOBJ/PNG VBVIBV/gpuHandle
	
	// UseRenderingEngine
#define __USE_RENDERING_ENGINE_DIRECTX12
//#define __USE_RENDERING_ENGINE_VULKAN
	
	// FixDeltaSeconds
	static constexpr float FixDeltaSeconds{ 1.0f / 60.0f };

	extern inline const std::filesystem::path LogFileName{
		std::format(L"{:%F-%H%M%S}.log", ChronoUtility::NowLocalSecond())
	};

	extern inline const std::filesystem::path LogFilePath{
		std::format(L"./Log/{}", EngineSettings::LogFileName.native())
	};
};