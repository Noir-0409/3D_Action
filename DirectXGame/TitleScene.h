#pragma once
#include "KamataEngine.h"
#include "input/Input.h"
#include "TitleSkydome.h"

using namespace KamataEngine;

class TitleScene {

public:
	~TitleScene();

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

	TitleSkydome* titleSkydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

};