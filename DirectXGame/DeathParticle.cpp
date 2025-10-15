#include "DeathParticle.h"

void DeathParticle::Initialize(Model* model, Camera* camera, const Vector3& position) {

	particleModel_ = model;
	camera_ = camera;

	for (auto& worldTransform : worldTransforms_) {

		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
}

void DeathParticle::Update() {

	for (auto& worldTransform : worldTransforms_) {

		worldTransform.UpdateMatrix();
	}
}

void DeathParticle::Draw() {

for (const auto& worldTransform : worldTransforms_) {

		particleModel_->Draw(worldTransform, *camera_);
	}

}
