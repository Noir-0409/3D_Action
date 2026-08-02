#pragma once
#include "KamataEngine.h"

namespace KamataEngine {

// アフィン変換行列の作成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

// 行列の掛け算
Matrix4x4 MatrixMultiply(const Matrix4x4& m1, const Matrix4x4& m2);

} // namespace KamataEngine