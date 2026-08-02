#pragma once
#include "GameObject.h"
#include <KamataEngine.h>

namespace KamataEngine { // ★ namespace の開始

/// <summary>
/// 天球を管理するクラス（GameObjectを継承）
/// </summary>
class Skydome : public GameObject {

public:
	void Initialize(Model* model, const Camera* camera, const Vector3& position = {0.0f, 0.0f, 0.0f}) override;

	void Update() override;

	void Draw() override;

private:
	float rotationY_ = 0.0f;
};

} // namespace KamataEngine