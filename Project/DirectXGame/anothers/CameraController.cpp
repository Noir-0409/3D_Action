#define NOMINMAX

#include "CameraController.h"
#include "Player.h"
#include "MathUtillity.h"
using namespace KamataEngine::MathUtility;

void CameraController::Initialize() {

	camera_->Initialize();

}

void CameraController::Update() {

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	const Vector3 targetVelocity_ = target_->GetVelocity();

	targetPosition_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity_.x * kVeloicityBias;
	targetPosition_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity_.y * kVeloicityBias;
	targetPosition_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity_.z * kVeloicityBias;

	camera_->translation_.x = Lerp(camera_->translation_.x, targetPosition_.x, kInterpolationRate_);
	camera_->translation_.y = Lerp(camera_->translation_.y, targetPosition_.y, kInterpolationRate_);

	camera_->translation_.x = std::max(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, movableArea_.right);
	camera_->translation_.y = std::max(camera_->translation_.y, movableArea_.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, movableArea_.top);

	camera_->translation_.x = std::max(camera_->translation_.x, targetWorldTransform.translation_.x + margin_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, targetWorldTransform.translation_.x + margin_.right);
	camera_->translation_.y = std::max(camera_->translation_.y, targetWorldTransform.translation_.y + margin_.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, targetWorldTransform.translation_.y + margin_.top);

	camera_->UpdateMatrix();

}

void CameraController::Reset() {

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;

}
