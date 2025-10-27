#include "Skydome.h"
#include <DirectXMath.h>
using namespace DirectX;

void Skydome::Initialize(Model* model, Camera* camera) {

	worldTransform_.Initialize();
	model_ = model;
	camera_ = camera;

}

void Skydome::Update() {

	 rotationY_ += 0.002f;
	if (rotationY_ > XM_2PI)
		rotationY_ -= XM_2PI;

	worldTransform_.rotation_.y = rotationY_;
	worldTransform_.UpdateMatrix(); 

}

void Skydome::Draw() {

	model_->Draw(worldTransform_, *camera_);

}
