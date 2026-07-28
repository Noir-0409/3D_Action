#include "AABB.h"

namespace KamataEngine { // ★ namespace の開始

bool AABB::IsCollision(const AABB& aabb1, const AABB& aabb2) {

	// X軸方向の判定
	if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x) {
		return false;
	}
	// Y軸方向の判定
	if (aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y) {
		return false;
	}
	// Z軸方向の判定
	if (aabb1.max.z < aabb2.min.z || aabb1.min.z > aabb2.max.z) {
		return false;
	}
	// 全ての軸で交差している場合
	return true;
}

} // namespace KamataEngine