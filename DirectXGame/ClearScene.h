#pragma once
#include "KamataEngine.h"
#include "input/Input.h"

namespace KamataEngine { // ★ namespace の開始

/// <summary>
/// クリアシーンを管理するクラス
/// </summary>
class ClearScene {

public:
	void Initialize();

	void Update();

	void Draw();

	// シーンを終了
	bool IsFinished() { return isFinished_; }

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	Camera camera_; // namespace 内なので KamataEngine:: の修飾は不要になります

	bool isFinished_ = false;
};

} // namespace KamataEngine