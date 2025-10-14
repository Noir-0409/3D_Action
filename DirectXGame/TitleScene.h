#pragma once
#include "KamataEngine.h"
#include "TitleSkydome.h"
#include "input/Input.h"

using namespace KamataEngine;

class TitleScene {

public:
	~TitleScene();

	void Initialize();
	void Update(float deltaTime);
	void Draw();

	bool IsFinished() { return isFinished_; }

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	KamataEngine::Camera camera_;

	bool isFinished_ = false;

	TitleSkydome* titleSkydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	uint32_t titleTextureHandle_ = 0;
	Sprite* titleSprite_ = nullptr;

	uint32_t startTextureHandle_ = 0;
	Sprite* startSprite_ = nullptr;

	Vector2 spritePos_ = {0, 0};
	Vector2 startPos_ = {0, 0};

	float startAlpha_ = 1.0f; // スタートスプライト透明度
	float alphaDir_ = -0.02f;

	// フェード用
	Sprite* fadeSprite_ = nullptr;
	float fadeAlpha_ = 1.0f;    // 1.0=黒, 0.0=透明
	float fadeDuration_ = 1.0f; // フェードにかける時間（秒）
	float fadeTimer_ = 0.0f;

	enum class FadeState { None, FadeIn, FadeOut };
	FadeState fadeState_ = FadeState::FadeIn;
};