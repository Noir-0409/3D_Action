#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

// Vector3の足し算
Vector3& operator+=(Vector3& lhv, const Vector3& rhv);

 Vector3 operator+(const Vector3& lhs, const Vector3& rhs);

 Vector3 operator*(const Vector3& vec, float scalar);