#pragma once
#include "KamataEngine.h"

namespace KamataEngine { // ★ namespace の開始

// Vector3の計算
Vector3& operator+=(Vector3& lhv, const Vector3& rhv);

Vector3 operator+(const Vector3& lhs, const Vector3& rhs);

Vector3 operator*(const Vector3& vec, float scalar);

} // namespace KamataEngine