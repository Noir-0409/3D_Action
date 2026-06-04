#pragma once
#include <KamataEngine.h>
#include "MapChip.h"
#include "3D/Model.h"

using namespace KamataEngine;

class BlockChip : public MapChip {
public:
	BlockChip(Model* model) : model_(model) {}

	void Draw(WorldTransform& wt, Camera& camera) override { model_->Draw(wt, camera); }

private:
	Model* model_;
};