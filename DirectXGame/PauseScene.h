#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class PauseScene {

public:
	void Initialize();

	void Update();

	void Draw();

private:

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
};