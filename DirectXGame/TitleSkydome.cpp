#include "TitleSkydome.h"
#include <DirectXMath.h>
using namespace DirectX;

void TitleSkydome::Initialize(Model* model, Camera* camera) {

	worldTransform_.Initialize();
	model_ = model;
	camera_ = camera;

}

void TitleSkydome::Update() {

	// Y軸方向にゆっくり回転させる
	 rotationY_ -= 0.002f;

	// 回転角が2πを超えたら範囲内に戻す
	if (rotationY_ > XM_2PI)
		rotationY_ -= XM_2PI;

	// 回転角をワールド変換に反映
	worldTransform_.rotation_.y = rotationY_;
	worldTransform_.UpdateMatrix(); 

}

void TitleSkydome::Draw() {

	model_->Draw(worldTransform_, *camera_);

}
