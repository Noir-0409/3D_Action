#pragma once
#include "IScene.h" // ★親クラス（インターフェース）を必ずインクルード
#include "KamataEngine.h"
#include "TitleSkydome.h"

using namespace KamataEngine;

/// <summary>
/// タイトルシーンを管理するクラス（ISceneをしっかりと継承！）
/// </summary>
class TitleScene : public IScene { // ★「: public IScene」でポリモーフィズムの対象にします

public:
	// ★仮想デストラクタとしてオーバーライド（メモリリークを防ぐ設計）
	~TitleScene() override;

	// ★親クラスの純粋仮想関数を上書きするため、すべて「override」を明示します
	void Initialize() override;

	void Update(float deltaTime) override;

	void Draw() override;

	bool IsFinished() const override { return isFinished_; }

private:
	// フェードの状態管理用
	enum class FadeState { None, FadeIn, FadeOut };
	FadeState fadeState_ = FadeState::FadeIn; // 初期状態はフェードイン

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	Camera camera_;

	Model* modelSkydome_ = nullptr;
	TitleSkydome* titleSkydome_ = nullptr;

	uint32_t titleTextureHandle_ = 0;
	Sprite* titleSprite_ = nullptr;

	uint32_t startTextureHandle_ = 0;
	Sprite* startSprite_ = nullptr;

	Sprite* fadeSprite_ = nullptr;

	Vector2 spritePos_ = {0.0f, 0.0f};
	Vector2 startPos_ = {0.0f, 0.0f};

	float totalTime_ = 0.0f;
	float fadeTimer_ = 0.0f;
	float fadeAlpha_ = 1.0f;    // 最初は真っ黒からスタート
	float fadeDuration_ = 1.0f; // 1秒かけてフェード
	float startAlpha_ = 1.0f;

	bool isFinished_ = false; // ★ISceneの純粋仮想関数 IsFinished() が返すフラグ
};