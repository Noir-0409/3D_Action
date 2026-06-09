#pragma once
#include "GameObject.h" // ★これを追加！
#include <KamataEngine.h>

using namespace KamataEngine;

/// <summary>
/// 天球を管理するクラス（GameObjectを継承）
/// </summary>
class Skydome : public GameObject { // ★「: public GameObject」を追加して継承させる

public:
	// ★ 引数を親のルールに合わせて「const Camera* camera」に変更し、位置情報の「Vector3」も追加。さらに「override」を付けます
	void Initialize(Model* model, const Camera* camera, const Vector3& position = {0.0f, 0.0f, 0.0f}) override;

	// ★「override」を追加
	void Update() override;

	// ★「override」を追加（引数なしで親クラスのDrawを上書きします）
	void Draw() override;

private:
	// ★「worldTransform_」「model_」「camera_」は
	// 親クラス（GameObject）がすでに持っているので、二重定義エラーを防ぐためにここからは削除します！

	float rotationY_ = 0.0f;
};