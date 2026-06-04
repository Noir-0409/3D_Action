#include "GameScene.h"
#include "IScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
#include <chrono>

using namespace KamataEngine;

IScene* currentScene = nullptr;
bool isGameScene = false;

void ChangeScene();
void UpdateScene(float deltaTime);
void DrawScene();

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"Action_Run");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	currentScene = new TitleScene();
	currentScene->Initialize();

	auto previousTime = std::chrono::high_resolution_clock::now();

	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

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

	delete currentScene;
	currentScene = nullptr;

	KamataEngine::Finalize();
	return 0;
}

void ChangeScene() {

	if (!currentScene) {
		return;
	}

	if (!currentScene->IsFinished()) {
		return;
	}

	delete currentScene;
	currentScene = nullptr;

	if (!isGameScene) {

		currentScene = new GameScene();
		currentScene->Initialize();

		isGameScene = true;
	} else {

		currentScene = new TitleScene();
		currentScene->Initialize();

		isGameScene = false;
	}
}

void UpdateScene(float deltaTime) {

	if (currentScene) {
		currentScene->Update(deltaTime);
	}
}

void DrawScene() {

	if (currentScene) {
		currentScene->Draw();
	}
}