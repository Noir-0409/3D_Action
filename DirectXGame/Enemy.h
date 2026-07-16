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
class Enemy : public GameObject { // ★「: public GameObject」を追加して継承させる

public:
	// ★ 引数を親のルールに合わせて「const Camera* camera」に変更し、「override」を追加
	void Initialize(Model* model, const Camera* camera, const Vector3& position) override;

	// ★「override」を追加
	void Update() override;

	// ★「override」を追加（引数なしで親クラスのDrawを上書きします）
	void Draw() override;

	// マップチップの特定の座標に配置する
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 立方体の当たり判定
	AABB GetAABB();

	// 当たっているかどうか
	void OnCollision(const Player* player);

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