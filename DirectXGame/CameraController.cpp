#define NOMINMAX

#include "CameraController.h"
#include "MathUtillity.h"
#include "Player.h"

namespace KamataEngine { // ★ namespace の開始

using namespace KamataEngine::MathUtility;

void CameraController::Initialize() { camera_->Initialize(); }

void CameraController::Update() {
	if (target_ == nullptr)
		return; // 安全のための防衛コード

	// ターゲットのワールド変換情報を取得（GameObjectから継承した関数を安全に呼び出します）
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// ターゲットの移動速度を取得
	const Vector3 targetVelocity_ = target_->GetVelocity();

	// カメラが目標とする位置を計算
	// ターゲット位置 + オフセット + 速度による先読み補正
	targetPosition_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity_.x * kVeloicityBias;
	targetPosition_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity_.y * kVeloicityBias;
	targetPosition_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity_.z * kVeloicityBias;

	// 線形補間でカメラを滑らかに追従させる
	camera_->translation_.x = Lerp(camera_->translation_.x, targetPosition_.x, kInterpolationRate_);
	camera_->translation_.y = Lerp(camera_->translation_.y, targetPosition_.y, kInterpolationRate_);

	// カメラが移動できるエリアの制限
	camera_->translation_.x = std::max(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, movableArea_.right);
	camera_->translation_.y = std::max(camera_->translation_.y, movableArea_.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, movableArea_.top);

	// ターゲットとの距離制限
	// ターゲットから一定距離以上離れないようにする
	camera_->translation_.x = std::max(camera_->translation_.x, targetWorldTransform.translation_.x + margin_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, targetWorldTransform.translation_.x + margin_.right);
	camera_->translation_.y = std::max(camera_->translation_.y, targetWorldTransform.translation_.y + margin_.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, targetWorldTransform.translation_.y + margin_.top);

	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	if (target_ == nullptr)
		return; // 安全のための防衛コード

	// ターゲットの現在位置を取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// ターゲット位置 + オフセットにカメラを即座に移動
	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}

} // namespace KamataEngine