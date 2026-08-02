#pragma once
#include "KamataEngine.h"

namespace KamataEngine { // ★ namespace の開始

/// <summary>
/// playerとenemyの当たり判定を管理するクラス
/// </summary>
class AABB {
public:
	KamataEngine::Vector3 min; // ★ 明示的に KamataEngine:: を付与
	KamataEngine::Vector3 max; // ★ 明示的に KamataEngine:: を付与

	// 立方体の当たり判定
	static bool IsCollision(const AABB& aabb1, const AABB& aabb2);
};

} // namespace KamataEngine