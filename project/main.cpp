#include "Engine/Application/WinApp.h"

#include "Application/Scene/DebugScene.h"
#include "Application/Scene/SelectScene.h"
#include "Application/Scene/TitleScene.h"

#include <Engine/Resources/Audio/AudioManager.h>

#include "Engine/Runtime/Scene/SceneManager.h"
#include "TestCode/SceneDemo.h"

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	WinApp::Initialize();

#ifdef _DEBUG
	WorldClock::IsFixDeltaTime(false);
#endif // _DEBUG

#ifdef _DEBUG
	SceneManager::Initialize(std::make_unique<TitleScene>());
#else
	SceneManager::Initialize(std::make_unique<SelectScene>());
#endif // _DEBUG

	AudioManager::SetMasterVolume(0.3f);

	WinApp::ShowAppWindow();

	while(true) {
		WinApp::BeginFrame();

		SceneManager::Begin();

		WinApp::ProcessMessage();

		if(WinApp::IsEndApp()) {
			break;
		}

		SceneManager::Update();

		SceneManager::Draw();

#ifdef _DEBUG
		SceneManager::DebugGui();
#endif // _DEBUG

		WinApp::EndFrame();
	}

	WinApp::Finalize();
}
