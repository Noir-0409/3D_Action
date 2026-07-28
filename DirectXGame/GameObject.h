#pragma once

#include "3D/Model.h"
#include "CameraController.h"
#include "KamataEngine.h"

namespace KamataEngine { // ★ namespace の開始

class GameObject {
protected:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	const KamataEngine::Camera* camera_ = nullptr;
	bool isDead_ = false;

public:
	// ★先生のアドバイス：メモリリークを防ぐための「仮想デストラクタ」
	virtual ~GameObject() = default;

	// 共通の初期化・更新・描画メソッド（仮想関数）
	virtual void Initialize(KamataEngine::Model* model, const KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
		model_ = model;
		camera_ = camera;
		worldTransform_.Initialize();
		worldTransform_.translation_ = position;
	}

	virtual void Update() {
		// 基本的な行列更新。子クラスで上書き(override)も可能
		worldTransform_.UpdateMatrix();
	}

	virtual void Draw() {
		if (model_ && camera_) {
			model_->Draw(worldTransform_, *camera_);
		}
	}

	// ゲッター・セッター（必要に応じて子クラスやGameSceneから使う）
	const KamataEngine::Vector3& GetWorldPosition() const { return worldTransform_.translation_; }
	bool IsDead() const { return isDead_; }

	// GameObject.h の public: の中に追加してください
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
};

} // namespace KamataEngine