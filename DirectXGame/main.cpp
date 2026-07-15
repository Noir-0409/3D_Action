#include "GameScene.h"
#include "IScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
#include <chrono>
#include <memory> // ★ std::unique_ptr を使用するために追加

using namespace KamataEngine;

// ⭕ 指摘事項: 生ポインタを std::unique_ptr に変更
// これにより、手動で delete する必要が完全になくなります
std::unique_ptr<IScene> currentScene = nullptr;

// 次にどのシーンを作るかを判定するための状態管理
enum class SceneType { kTitle, kGame };
SceneType currentSceneType = SceneType::kTitle;

void ChangeScene();

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"Action_Run");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ⭕ 指摘事項: new を排除し、std::make_unique でタイトルシーンを生成
	currentScene = std::make_unique<TitleScene>();
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

	// ⭕ 指摘事項: 手動の delete は不要になったため完全に削除！
	// currentScene = nullptr; // unique_ptr なので何もしなくても自動で安全に解放されます

	KamataEngine::Finalize();
	return 0;
}

// シーン切り替えのロジック
void ChangeScene() {
	if (currentScene && currentScene->IsFinished()) {

		switch (currentSceneType) {
		case SceneType::kTitle:
			// ⭕ 指摘事項: 手動 delete を排除。
			// 新しいシーンを std::make_unique で代入すると、古いシーンは自動的かつ安全に破棄（delete）されます
			currentScene = std::make_unique<GameScene>();
			currentScene->Initialize();
			currentSceneType = SceneType::kGame;
			break;

		case SceneType::kGame:
			// ⭕ 指摘事項: 同様に、代入するだけで古い GameScene は自動で破棄されます
			currentScene = std::make_unique<TitleScene>();
			currentScene->Initialize();
			currentSceneType = SceneType::kTitle;
			break;
		}
	}
}