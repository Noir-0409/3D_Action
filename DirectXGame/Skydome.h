#pragma once
#include <KamataEngine.h>

using namespace KamataEngine;

/// <summary>
/// 天球を管理するクラス
/// </summary>
class Skydome {

public:

void Initialize(Model* model,Camera* camera);

void Update();

void Draw();

private:

WorldTransform worldTransform_;

Model* model_ = nullptr;

Camera* camera_ = nullptr;

float rotationY_ = 0.0f;

};