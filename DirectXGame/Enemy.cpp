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
	worldTransform_.translation_.y += velocity_.y;

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

Vector3 Enemy::GetWorldPosition() { 
	
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得
	// ワールド行列のTx
	worldPos.x = worldTransform_.translation_.x;

	// ワールド行列のTy
	worldPos.y = worldTransform_.translation_.y;

	// ワールド行列のTz
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
	
	 }

AABB Enemy::GetAABB() { 

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};

	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;

}

void Enemy::OnCollision(const Player* player) {

	(void)player;
	velocity_.x += 0.01f;
	velocity_.y += 0.01f;
}
