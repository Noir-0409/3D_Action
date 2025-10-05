#include "Skydome.h"

void Skydome::Initialize(Model* model, Camera* camera) {

	worldTransform_.Initialize();
	model_ = model;
	camera_ = camera;

}

void Skydome::Update() {}

void Skydome::Draw() {

	model_->Draw(worldTransform_, *camera_);

}
