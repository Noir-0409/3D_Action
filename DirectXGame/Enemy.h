#pragma once
#include "AABB.h"
#include "GameObject.h"
#include "KamataEngine.h"
#include "MapChipField.h"

using namespace KamataEngine;

class Player;

/// <summary>
/// enemyを管理するクラス（GameObjectを継承）
/// </summary>
class Enemy : public GameObject {

public:
	
	void Initialize(Model* model, const Camera* camera, const Vector3& position) override;

	void Update() override;

	void Draw() override;

	// 立方体の当たり判定
	AABB GetAABB();

	// 当たっているかどうか
	void OnCollision(const Player* player);

	// マップチップの特定の座標に配置する
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

private:

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