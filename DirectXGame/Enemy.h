#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"

using namespace KamataEngine;

class Enemy {

public:

void Initialize(Model* model, Camera* viewProjection, const Vector3& position);

void Update();

void Draw();

void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

private:

WorldTransform worldTransform_;

Model* model_ = nullptr;

Camera* camera_ = nullptr;

MapChipField* mapChipField_ = nullptr;

static inline const float kWalkSpeed = 0.04f;

Vector3 velocity_ = {};

static inline const float kWalkMotionAngleStart = 0.0f;

static inline const float kWalkMotionAngleEnd = 0.3f;

static inline const float kWalkMotionTime = 1.0f;

float walkTimer_ = 0.0f;

};