#pragma once
#include <KamataEngine.h>
using namespace KamataEngine;

class TitleSkydome {

public:

void Initialize(Model* model, Camera* camera);

void Update();

void Draw();

private:

WorldTransform worldTransform_;

	Model* model_ = nullptr;

	Camera* camera_ = nullptr;

};