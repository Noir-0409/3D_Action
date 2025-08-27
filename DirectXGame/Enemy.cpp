#include "Enemy.h"
#include <numbers>

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {

	worldTransform_.Initialize();
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = position;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;

}

void Enemy::Update() {

	worldTransform_.UpdateMatrix();

}

void Enemy::Draw() {

	model_->Draw(worldTransform_, *camera_);

}
