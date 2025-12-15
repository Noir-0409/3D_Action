#include "MathUtillity.h"

Vector3& operator+=(Vector3& lhv, const Vector3& rhv) {

	lhv.x += rhv.x;
	lhv.y += rhv.y;
	lhv.z += rhv.z;

	return lhv;
}

Vector3 operator+(const Vector3& lhs, const Vector3& rhs) {
	Vector3 temp = lhs;
	temp += rhs; // operator+= を使う
	return temp;
}

Vector3 operator*(const Vector3& vec, float scalar) { return {vec.x * scalar, vec.y * scalar, vec.z * scalar}; }
