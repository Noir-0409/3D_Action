#include "GameScene.h"
#include "IScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
#include <chrono>
#include <memory>

// main.cpp 内でのみ使用する変数や関数は無名名前空間でカプセル化
namespace {

enum class SceneType { kTitle, kGame };

std::unique_ptr<KamataEngine::IScene> currentScene = nullptr;
SceneType currentSceneType = SceneType::kTitle;

void ChangeScene() {
	if (currentScene && currentScene->IsFinished()) {
		switch (currentSceneType) {
		case SceneType::kTitle:
			currentScene = std::make_unique<KamataEngine::GameScene>();
			currentScene->Initialize();
			currentSceneType = SceneType::kGame;
			break;

		case SceneType::kGame:
			currentScene = std::make_unique<KamataEngine::TitleScene>();
			currentScene->Initialize();
			currentSceneType = SceneType::kTitle;
			break;
		}
	}
}

} // namespace

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"Action_Run");
	KamataEngine::DirectXCommon* dxCommon = KamataEngine::DirectXCommon::GetInstance();

	// std::make_unique でタイトルシーンを生成
	currentScene = std::make_unique<KamataEngine::TitleScene>();
	currentScene->Initialize();
	currentSceneType = SceneType::kTitle;

	auto previousTime = std::chrono::high_resolution_clock::now();

	while (true) {

		if (KamataEngine::Update())
			break;

		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = currentTime - previousTime;
		previousTime = currentTime;
		float deltaTime = elapsed.count();

		// 今が何のシーンかに関わらず、現在のシーンを更新
		if (currentScene) {
			currentScene->Update(deltaTime);
		}

		// シーン切り替えチェック
		ChangeScene();

		dxCommon->PreDraw();

		// 今が何のシーンかに関わらず、現在のシーンを描画
		if (currentScene) {
			currentScene->Draw();
		}

		dxCommon->PostDraw();
	}

	KamataEngine::Finalize();
	return 0;
}