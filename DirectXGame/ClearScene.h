#pragma once
#include "KamataEngine.h"
#include "input/Input.h"

using namespace KamataEngine;

/// <summary>
///クリアシーンを管理するクラス 
/// </summary>
class ClearScene {

public:
	void Initialize();

	void Update();

	void Draw();

	bool IsFinished() { return isFinished_; }

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	KamataEngine::Camera camera_;

	bool isFinished_ = false;
};