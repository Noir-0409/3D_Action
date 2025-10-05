#include "GameScene.h"
#include "TitleScene.h"
#include "KamataEngine.h"
#include <Windows.h>

using namespace KamataEngine;

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;

enum class Scene {

	kUnknown = 0,

	kTitle,
	kGame

};

Scene scene = Scene::kUnknown;

void ChangeScene();

void UpdateScene();

void DrawScene();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LE3C_15_タムラ_アツキ");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	titleScene = new TitleScene();
	titleScene->Initialize();

	scene = Scene::kTitle;

	gameScene = new GameScene();
	gameScene->Initialize();

	// メインループ
	while (true) {

		// エンジンの初期化
		if (KamataEngine::Update()) {

			break;
		}

		// ゲームシーンの更新
		UpdateScene();
		ChangeScene();

		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		DrawScene();

		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete titleScene;
	delete gameScene;

	// nullptrの代入
	gameScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}

void ChangeScene() {

	switch (scene) {

	case Scene::kTitle:

		if (titleScene->IsFinished()) {

			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;
			gameScene = new GameScene();
			gameScene->Initialize();
		}

		break;

	case Scene::kGame:

		if (gameScene->IsFinished()) {

			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}

		break;
	}


}

void UpdateScene() {

	switch (scene) {

	case Scene::kTitle:

		titleScene->Update();

		break;

	case Scene::kGame:

		gameScene->Update();

		break;
	}

}

void DrawScene() {

	switch (scene) {

	case Scene::kTitle:

		titleScene->Draw();

		break;

	case Scene::kGame:

		gameScene->Draw();

		break;
	}

}
