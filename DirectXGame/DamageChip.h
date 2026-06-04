#pragma once


#include "MapChip.h"
#include "3D/Model.h"

using namespace KamataEngine;

class DamageChip : public MapChip {
public:
	DamageChip(Model* model, uint32_t tex1, uint32_t tex2) : model_(model), tex1_(tex1), tex2_(tex2) {}

	void SetFireToggle(bool flag) { fireToggle_ = flag; }

	void Draw(WorldTransform& wt, Camera& camera) override {
		if (fireToggle_) {
			model_->Draw(wt, camera, tex1_);
		} else {
			model_->Draw(wt, camera, tex2_);
		}
	}

private:
	Model* model_ = nullptr;
	uint32_t tex1_ = 0;
	uint32_t tex2_ = 0;
	bool fireToggle_ = false;
};