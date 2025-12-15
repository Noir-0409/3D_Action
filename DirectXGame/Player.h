#pragma once
#include "KamataEngine.h"
#include "AABB.h"

using namespace KamataEngine;

class MapChipField;
class Enemy;

enum class LRDirection {

	kRight,
	kLeft,

};

/// <summary>
/// playerを管理するクラス
/// </summary>
class Player {

public:

	struct CollisionMapInfo {

		bool ceiling = false;
		bool landing = false;
		bool hitwall = false;
		Vector3 move;
	};

	enum Corner {

		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,

		kNumCorner // 要素数

	};

	~Player();

	void Initialize(Model* model, Camera* camera, const Vector3& position);

	void Update();

	void Draw(Camera& camera);

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void InputMove();

	void CollisionMove(const CollisionMapInfo& info);

	void CheckMapCollision(CollisionMapInfo& info);

	void CheckMapCollisionUp(CollisionMapInfo& info);

	void CheckMapCollisionDown(CollisionMapInfo& info);

	void CheckMapCollisionLeft(CollisionMapInfo& info);

	void CheckMapCollisionRight(CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void UpdateOnGround(const CollisionMapInfo& info);

	void UpdateRotation();

	void UpdateRotationSmooth();

	void UpdateHitWall(const CollisionMapInfo& info);

	Vector3 GetWorldPosition() const;

	AABB GetAABB();

	void OnCollision(const Enemy* enemy);

	bool isDead_ = false;

	bool IsDead() const { return isDead_; }

	bool inputEnabled_ = true;

	 void SetInputEnabled(bool enabled) { inputEnabled_ = enabled; }

	 bool IsGoal() const { return isGoal_; }

	 void StartDeathFall();

	 bool IsFalling() const { return isFalling_; }

private:
	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	Input* input_ = nullptr;

	Vector3 velocity_ = {};

	Camera* camera_ = nullptr;

	static inline const float kAcceleration = 0.003f;
	static inline const float kLimitRunSpeed = 0.5f;
	static inline const float kAttenuation = 0.1f;

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;

	static inline const float kGravityAcceleration = 0.05f;
	static inline const float kLimitFallSpeed = 1.0f;
	static inline const float kJumpAcceleration = 0.4f;

	MapChipField* mapChipField_ = nullptr;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.04f;

	static inline const float kAttenuationLanding = 0.0f;

	static inline const float kGroundSearchHeight = 0.01f;

	static inline const float kAttenuationWall = 0.00001f;

	bool isAttacking_ = false;
	int attackTimer_ = 0; 

	float jumpTime_ = 0.0f;
	static inline const float kMaxJumpTime = 0.25f;

	bool isGoal_ = false;

	bool isFalling_ = false;
	Vector3 deathFallVelocity_ = {0.0f, -5.0f, 0.0f};
	float deathRotationSpeed_ = 180.0f; 

	float deathVelocityY_ = 0.0f; // Y方向速度
	const float gravity_ = -0.01f;

};