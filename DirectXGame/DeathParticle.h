#pragma once
#include "KamataEngine.h"
#include <array>

using namespace KamataEngine;

class DeathParticle {

public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	void Update();

	void Draw();

private:

Model* particleModel_ = nullptr;  
Camera* camera_ = nullptr;

static inline const uint32_t kNumParticles = 8;

std::array<WorldTransform, kNumParticles> worldTransforms_;

};