#include "FireParticle.h"

void FireParticle::Initialize(uint32_t textureHandle, const Vector2& origin, int count) {
	textureHandle_ = textureHandle;
	particles_.clear();

	for (int i = 0; i < count; ++i) {
		Particle p;
		p.position = origin;
		// 上に上がる速度をランダムに
		p.velocity = {0.0f, -(float)(rand() % 20 + 20) / 60.0f};
		p.sprite = nullptr; // 初期化時は nullptr
		particles_.push_back(p);
	}
}

void FireParticle::Update() {
	for (auto& p : particles_) {
		p.position.y += p.velocity.y;

		// 上に移動したら下に戻す（ループさせる）
		if (p.position.y < -100.0f) { // 画面外でリセット
			p.position.y += 50.0f;    // 適当な値でリセット
		}
	}
}

void FireParticle::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Sprite::PreDraw(dxCommon->GetCommandList());

	for (auto& p : particles_) {
		if (!p.sprite) {
			p.sprite = Sprite::Create(textureHandle_, p.position);
		} else {
			p.sprite->SetPosition(p.position);
		}
		p.sprite->Draw();
	}

	Sprite::PostDraw();
}
