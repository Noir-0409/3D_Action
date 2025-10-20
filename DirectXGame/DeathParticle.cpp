#include "DeathParticle.h"

using namespace KamataEngine;

void DeathParticle::Initialize(Model* model, Camera* camera, const Vector3& position) {

	particleModel_ = model;
	camera_ = camera;

	for (auto& worldTransform : worldTransforms_) {

		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
}

void DeathParticle::Update() {

	if (isFinished_) {

		return;
	}

	for (auto& worldTransform : worldTransforms_) {

		worldTransform.UpdateMatrix();
	}

	for (uint32_t i = 0; i < 8; ++i) {

		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0, 0};

		// 回転角を計算
		float angle = kAngleUint * i;

		// Z軸まわり回転行列
		Matrix4x4 matrixRotation = MathUtility::MakeRotateZMatrix(angle);

		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = MathUtility::Transform(velocity, matrixRotation);

		// 移動処理
		worldTransforms_[i].translation_ += velocity;
	}

	counter_ += 1.0f / 60.0f;

	// 存続時間の上限に達したら
	if (counter_ >= kDuration) {

		counter_ = kDuration;

		// 終了扱いにする
		isFinished_ = true;
	}

}

void DeathParticle::Draw() {

	if (isFinished_) {

		return;
	}

for (const auto& worldTransform : worldTransforms_) {

		particleModel_->Draw(worldTransform, *camera_);
	}

}
