#include "Skydome.h"
#include <DirectXMath.h>

namespace KamataEngine { // ★ namespace の開始

// ★ 引数に「const」と「position」を追加
void Skydome::Initialize(Model* model, const Camera* camera, const Vector3& position) {

	worldTransform_.Initialize();
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = position; // 渡された座標を設定（デフォルトは0,0,0）

	rotationY_ = 0.0f;
}

void Skydome::Update() {

	// Y軸方向にゆっくり回転させる
	rotationY_ -= 0.002f;

	// 回転角が2πを超えたら範囲内に戻す（DirectX::XM_2PI を明示）
	if (rotationY_ > DirectX::XM_2PI)
		rotationY_ -= DirectX::XM_2PI;

	// 回転角をワールド変換に反映
	worldTransform_.rotation_.y = rotationY_;
	worldTransform_.UpdateMatrix();
}

// ★ 親クラスのルールに合わせて引数なしで実装（内部に保持している camera_ を使用）
void Skydome::Draw() {
	if (model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

} // namespace KamataEngine