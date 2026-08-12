#pragma once
#include "AABB.h"
#include "GameObject.h"
#include "KamataEngine.h"
#include "PlayerState.h" // ★ 追加
#include <memory>        // ★ 追加

namespace KamataEngine { // ★ namespace の開始

class MapChipField;
class Enemy;

enum class LRDirection {
	kRight,
	kLeft,
};

class Player : public GameObject {
public:
	// ★ 地面探索用の高度定数を public へ移動（Player.cpp 等から参照可能にします）
	static inline const float kGroundSearchHeight = 0.01f;

	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitwall = false;
		Vector3 move;
	};

	enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };

	~Player() override;
	void Initialize(Model* model, const Camera* camera, const Vector3& position) override;
	void Update() override;
	void Draw() override;

	// ★ Stateパターン用の状態遷移メソッド
	void ChangeState(std::unique_ptr<PlayerState> newState);

	// --- 状態クラスからアクセスするためのヘルパー関数群 ★ ---
	const Vector3& GetVelocity() const { return velocity_; }
	void SetVelocity(const Vector3& v) { velocity_ = v; }
	bool IsAttacking() const { return isAttacking_; }
	void DecrementAttackTimer() {
		if (attackTimer_ > 0)
			--attackTimer_;
		else
			isAttacking_ = false;
	}

	void ApplyDeathGravity() {
		deathVelocityY_ += gravity_;
		worldTransform_.translation_.y += deathVelocityY_;
	}
	void UpdateDeathRotation(float deltaTime) { worldTransform_.rotation_.z += deathRotationSpeed_ * deltaTime; }
	void StopDeathFall() { isFalling_ = false; } // 互換性維持のため残す、または移行

	// (既存のパブリック関数はそのまま維持)
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
	void UpdateHitWall(const CollisionMapInfo& info);
	AABB GetAABB();
	void OnCollision(const Enemy* enemy);

	bool inputEnabled_ = true;
	void SetInputEnabled(bool enabled) { inputEnabled_ = enabled; }
	bool IsGoal() const { return isGoal_; }
	void StartDeathFall();
	bool IsFalling() const { return isFalling_; }
	bool IsOnIce() const;

	bool wasMovingInput_ = false;

private:
	// ★ 現在の状態を管理するスマートポインタ
	std::unique_ptr<PlayerState> state_;

	Input* input_ = nullptr;
	Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.003f;
	static inline const float kLimitRunSpeed = 0.5f;
	static inline const float kAttenuation = 0.1f;

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;

	static inline const float kGravityAcceleration = 0.03f;
	static inline const float kLimitFallSpeed = 1.0f;
	static inline const float kJumpAcceleration = 0.3f;

	MapChipField* mapChipField_ = nullptr;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.04f;
	static inline const float kAttenuationLanding = 0.0f;
	static inline const float kAttenuationWall = 0.00001f;

	bool isAttacking_ = false;
	int attackTimer_ = 0;

	float jumpTime_ = 0.0f;
	static inline const float kMaxJumpTime = 0.25f;

	bool isGoal_ = false;

	bool isFalling_ = false;
	Vector3 deathFallVelocity_ = {0.0f, -5.0f, 0.0f};
	float deathRotationSpeed_ = 180.0f;

	float deathVelocityY_ = 0.0f;
	const float gravity_ = -0.01f;

	static inline const float kIceAcceleration = 0.006f;
	static inline const float kIceAttenuation = 0.001f;
	static inline const float kIceMaxSpeed = 0.8f;
};

} // namespace KamataEngine