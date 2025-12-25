#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;

/// <summary>
/// platerとenemyの当たり判定を管理するクラス
/// </summary>
class AABB {
public:
	Vector3 min;
	Vector3 max;

	//立方体の当たり判定
	static bool IsCollision(const AABB& aabb1, const AABB& aabb2);
};