#pragma once

#include "MapChip.h"

class EmptyChip : public MapChip {
public:
	void Draw(WorldTransform&, Camera&) override {
		//何も描画しない（空ブロック）
	}
};