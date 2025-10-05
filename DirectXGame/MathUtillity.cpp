#include "MathUtillity.h"

Vector3& operator+=(Vector3& lhv, const Vector3& rhv) {

	lhv.x += rhv.x;
	lhv.y += rhv.y;
	lhv.z += rhv.z;

	return lhv;
}
