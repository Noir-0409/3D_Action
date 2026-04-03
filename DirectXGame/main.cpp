#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
#include <chrono>

using namespace KamataEngine;

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;

enum class Scene { kUnknown = 0, kTitle, kGame, kClear };

Scene scene = Scene::kUnknown;

void ChangeScene();
void UpdateScene(float deltaTime);
void DrawScene();

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"Action_Run");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	titleScene = new TitleScene();
	titleScene->Initialize();
	scene = Scene::kTitle;

	gameScene = new GameScene();
	gameScene->Initialize();

	auto previousTime = std::chrono::high_resolution_clock::now();

	while (true) {

		if (KamataEngine::Update())
			break;

		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = currentTime - previousTime;
		previousTime = currentTime;
		float deltaTime = elapsed.count();

		UpdateScene(deltaTime);
		ChangeScene();

		dxCommon->PreDraw();
		DrawScene();
		dxCommon->PostDraw();
	}

	delete titleScene;
	delete gameScene;
	titleScene = nullptr;
	gameScene = nullptr;

	KamataEngine::Finalize();
	return 0;
}

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene && titleScene->IsFinished()) {
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;
			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene && gameScene->IsFinished()) {
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	}
}

void UpdateScene(float deltaTime) {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene)
			titleScene->Update(deltaTime);
		break;
	case Scene::kGame:
		if (gameScene)
			gameScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene)
			titleScene->Draw();
		break;
	case Scene::kGame:
		if (gameScene)
			gameScene->Draw();
		break;
	}
}
