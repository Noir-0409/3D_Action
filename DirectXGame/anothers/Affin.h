#pragma once
#include <math/Matrix4x4.h>
#include <math/Vector3.h>

// アフィン変換行列の作成
KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rot, const KamataEngine::Vector3& translate);

// 行列の掛け算
KamataEngine::Matrix4x4 MatrixMultiply(KamataEngine::Matrix4x4& m1, KamataEngine::Matrix4x4& m2);