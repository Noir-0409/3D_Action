#pragma once
#include <KamataEngine.h>

using namespace KamataEngine;

class MapChip {
public:
	virtual ~MapChip() = default;

	virtual void Draw(WorldTransform& wt, Camera& camera) = 0;
};