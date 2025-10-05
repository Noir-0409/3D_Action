#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Player;

struct Rect {

	float left = 0.0f;
	float right = 1.0f;
	float bottom = 0.0f;
	float top = 1.0f;

};

class CameraController {

public:

void Initialize();

void Update();

void SetTarget(Player* target) { target_ = target; }

void SetCamera(Camera* camera) { camera_ = camera; }

void Reset();

void SetMovableArea(Rect area) { movableArea_ = area; }

private:

Camera* camera_ = nullptr;

Player* target_ = nullptr;

Vector3 targetOffset_ = {0, 0, -15.0f};

Rect movableArea_ = {0, 100, 0, 100};

Vector3 targetPosition_;

static inline const float kInterpolationRate_ = 0.3f;

static inline const float kVeloicityBias = 0.5f;

static inline const Rect margin_ = {-8.0f, 8.0f, -8.0f, 8.0f};

};