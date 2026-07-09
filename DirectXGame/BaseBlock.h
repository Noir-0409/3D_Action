#pragma once

#include "3D/Model.h"
#include "CameraController.h"
#include "KamataEngine.h"
#include <memory> // ★ std::unique_ptr を使うために追加

class BaseBlock {
protected:
	KamataEngine::Model* model_ = nullptr;
	// ⭕ 生ポインタから std::unique_ptr に変更
	std::unique_ptr<KamataEngine::WorldTransform> worldTransform_ = nullptr;

public:
	// ⭕ 引数で受け取る型を std::unique_ptr に合わせ、std::move で所有権を受け取る
	BaseBlock(KamataEngine::Model* model, std::unique_ptr<KamataEngine::WorldTransform> wt) : model_(model), worldTransform_(std::move(wt)) {}

	// ⭕ 仮想デストラクタ（スマートポインタが自動解放するため、deleteは完全に消去！）
	virtual ~BaseBlock() = default;

	virtual void Draw(const KamataEngine::Camera& camera) = 0;

	// ⭕ 外部（GameSceneなど）がポインタとして扱えるように、.get() を使って生ポインタとして返す
	KamataEngine::WorldTransform* GetWorldTransform() const { return worldTransform_.get(); }
};