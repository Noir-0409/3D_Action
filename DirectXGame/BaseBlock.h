#pragma once

#include "3D/Model.h"
#include "CameraController.h"
#include "KamataEngine.h"
#include <memory>

namespace KamataEngine { // ★ namespace の開始

class BaseBlock {
protected:
	Model* model_ = nullptr;
	std::unique_ptr<WorldTransform> worldTransform_ = nullptr;

public:
	BaseBlock(Model* model, std::unique_ptr<WorldTransform> wt) : model_(model), worldTransform_(std::move(wt)) {}

	virtual ~BaseBlock() = default;

	virtual void Draw(const Camera& camera) = 0;

	WorldTransform* GetWorldTransform() const { return worldTransform_.get(); }
};

} // namespace KamataEngine