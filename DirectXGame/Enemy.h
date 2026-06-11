#pragma once
#include "AABB.h"
#include "CollisionObserver.h" // Observerインターフェースのインクルード
#include "KamataEngine.h"
#include "MapChipField.h"

using namespace KamataEngine;

class Player;

/// <summary>
/// enemyを管理するクラス
/// </summary>
class Enemy : public CollisionObserver { // CollisionObserverを継承

public:
	void Initialize(Model* model, Camera* viewProjection, const Vector3& position);

	void Update();

	void Draw();

	// マップチップの特定の座標に配置する
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 座標を受け取る
	Vector3 GetWorldPosition();

	// 立方体の当たり判定
	AABB GetAABB();

	// 当たっているかどうか
	void OnCollision(const Player* player);

	// Observerインターフェースの関数をオーバーライド
	void OnPlayerEnemyCollision(Player* player, Enemy* enemy) override;

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

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
};