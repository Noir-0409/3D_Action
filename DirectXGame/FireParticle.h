#pragma once
#include "KamataEngine.h"
#include <vector>

using namespace KamataEngine;

struct Particle {
	Vector2 position;
	Vector2 velocity;
	Sprite* sprite = nullptr;
};

class FireParticle {
public:
	void Initialize(uint32_t textureHandle, const Vector2& origin, int count = 5);
	void Update();
	void Draw();

private:
	std::vector<Particle> particles_;
	uint32_t textureHandle_;
};
