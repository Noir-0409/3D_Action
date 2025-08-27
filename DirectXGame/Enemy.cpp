#include "Enemy.h"
#include <numbers>

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {

	worldTransform_.Initialize();
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = position;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;

	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;

}

void Enemy::Update() {

	worldTransform_.translation_.x += velocity_.x;

	walkTimer_ += 1.0f / 60.0f;

	float t = std::fmod(walkTimer_, kWalkMotionTime) / kWalkMotionTime;
	float param = std::sin(t * 2.0f * std::numbers::pi_v<float>);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = radian;

	worldTransform_.UpdateMatrix();

}

void Enemy::Draw() {

	model_->Draw(worldTransform_, *camera_);

}
