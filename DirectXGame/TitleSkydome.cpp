#include "TitleSkydome.h"
#include <DirectXMath.h>
using namespace DirectX;

void TitleSkydome::Initialize(Model* model, Camera* camera) {

	worldTransform_.Initialize();
	model_ = model;
	camera_ = camera;

}

void TitleSkydome::Update() {

	 rotationY_ += 0.005f;
	if (rotationY_ > XM_2PI)
		rotationY_ -= XM_2PI;

	worldTransform_.rotation_.y = rotationY_;
	worldTransform_.UpdateMatrix(); 

}

void TitleSkydome::Draw() {

	model_->Draw(worldTransform_, *camera_);

}
