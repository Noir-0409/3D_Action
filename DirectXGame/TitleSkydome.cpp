#include "TitleSkydome.h"

void TitleSkydome::Initialize(Model* model, Camera* camera) {

	worldTransform_.Initialize();
	model_ = model;
	camera_ = camera;

}

void TitleSkydome::Update() {}

void TitleSkydome::Draw() {

	model_->Draw(worldTransform_, *camera_);

}
