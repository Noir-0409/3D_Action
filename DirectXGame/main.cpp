#include "GameScene.h"
#include "IScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
#include <chrono>

using namespace KamataEngine;

// ★【ここがポリモーフィズム】現在アクティブなシーンを指す、共通のポインタ
IScene* currentScene = nullptr;

// 次にどのシーンを作るかを判定するための状態管理
enum class SceneType { kTitle, kGame };
SceneType currentSceneType = SceneType::kTitle;

void ChangeScene();

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"Action_Run");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 最初のシーン（タイトル）を生成して初期化
	currentScene = new TitleScene();
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

		// ★【switch文が消滅！】今が何のシーンかに関わらず、現在のシーンを更新
		if (currentScene) {
			currentScene->Update(deltaTime);
		}

		// シーン切り替えチェック
		ChangeScene();

		dxCommon->PreDraw();

		// ★【switch文が消滅！】今が何のシーンかに関わらず、現在のシーンを描画
		if (currentScene) {
			currentScene->Draw();
		}

		dxCommon->PostDraw();
	}

	// 最後に残ったシーンを安全に解放（virtualデストラクタなので適切に消えます）
	delete currentScene;
	currentScene = nullptr;

	KamataEngine::Finalize();
	return 0;
}

// シーン切り替えのロジック
void ChangeScene() {
	if (currentScene && currentScene->IsFinished()) {

		switch (currentSceneType) {
		case SceneType::kTitle:
			// タイトルが終わったら、ゲームシーンへ切り替え
			delete currentScene; // 古いシーンを破棄
			currentScene = new GameScene();
			currentScene->Initialize();
			currentSceneType = SceneType::kGame;
			break;

		case SceneType::kGame:
			// ゲームシーンが終わったら、タイトルシーンへ戻る
			delete currentScene; // 古いシーンを破棄
			currentScene = new TitleScene();
			currentScene->Initialize();
			currentSceneType = SceneType::kTitle;
			break;
		}
	}
}