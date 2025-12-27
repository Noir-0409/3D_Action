#include "Enemy.h"
#include <numbers>

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {

	worldTransform_.Initialize();
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = position;

	// 初期向きを設定
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;

	// 移動速度を設定（左方向へ移動）
	velocity_ = {-kWalkSpeed, 0, 0};

	// 歩行モーション用タイマーを初期化
	walkTimer_ = 0.0f;

}

void Enemy::Update() {

	//worldTransform_.translation_.x += velocity_.x;
	//worldTransform_.translation_.y += velocity_.y;

	//walkTimer_ += 1.0f / 60.0f;

	// 歩行モーション
	// sin波を使って滑らかな周期運動を作る
	float t = std::fmod(walkTimer_, kWalkMotionTime) / kWalkMotionTime;

	// -1.0 ～ 1.0 の範囲で変化するパラメータ
	float param = std::sin(t * 2.0f * std::numbers::pi_v<float>);

	// 回転角を開始角～終了角の範囲にマッピング	
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;

	// X軸回転として適用（歩行の揺れ表現）
	worldTransform_.rotation_.x = radian;

	worldTransform_.UpdateMatrix();

}

void Enemy::Draw() {

	//model_->Draw(worldTransform_, *camera_);

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

	// 敵のワールド座標を取得
	Vector3 worldPos = GetWorldPosition();

	// 中心座標から幅・高さを使ってAABBを生成
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};

	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;

}

void Enemy::OnCollision(const Player* player) {

	(void)player;

}
