#define NOMINMAX
#include "TitleSkydome.h"
#include <DirectXMath.h>

namespace KamataEngine { // ★ namespace の開始

void TitleSkydome::Initialize(Model* model, Camera* camera) {

	worldTransform_.Initialize();
	model_ = model;
	camera_ = camera;
}

void TitleSkydome::Update() {

	// Y軸方向にゆっくり回転させる
	rotationY_ -= 0.002f;

	// 回転角が2πを超えたら範囲内に戻す（DirectX::XM_2PI を明示）
	if (rotationY_ > DirectX::XM_2PI)
		rotationY_ -= DirectX::XM_2PI;

	// 回転角をワールド変換に反映
	worldTransform_.rotation_.y = rotationY_;
	worldTransform_.UpdateMatrix();
}

void TitleSkydome::Draw() {

	if (model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

} // namespace KamataEngine