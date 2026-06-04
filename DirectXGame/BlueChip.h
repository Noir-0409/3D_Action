#pragma once

#include "MapChip.h"
#include "3D/Model.h"

using namespace KamataEngine;

class BlueChip : public MapChip {
public:
	BlueChip(Model* model) : model_(model) {}

	void Draw(WorldTransform& wt, Camera& camera) override { model_->Draw(wt, camera); }

private:
	Model* model_ = nullptr;
};