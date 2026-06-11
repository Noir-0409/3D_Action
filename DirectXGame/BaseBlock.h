#pragma once

#include "KamataEngine.h"
#include "CameraController.h"
#include "3D/Model.h"

class BaseBlock {
protected:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform* worldTransform_ = nullptr;

public:
	
	BaseBlock(KamataEngine::Model* model, KamataEngine::WorldTransform* wt) : model_(model), worldTransform_(wt) {}

	
	virtual ~BaseBlock() {
		delete worldTransform_; // 生成された WorldTransform をここで安全に自動解放
	}

	virtual void Draw(const KamataEngine::Camera& camera) = 0;

	KamataEngine::WorldTransform* GetWorldTransform() const { return worldTransform_; }

};