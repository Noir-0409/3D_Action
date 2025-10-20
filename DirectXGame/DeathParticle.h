#pragma once
#include "KamataEngine.h"
#include <array>
#include "MathUtillity.h"

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

static inline const float kDuration = 1.0f;

static inline const float kSpeed = 0.1f;

static inline const float kAngleUint = 2.0f * 3.14f / 8.0f;

bool isFinished_ = false;

float counter_ = 0.0f;

};