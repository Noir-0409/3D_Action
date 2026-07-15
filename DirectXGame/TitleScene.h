#pragma once
#include "IScene.h"
#include "KamataEngine.h"
#include "TitleFadeState.h"
#include "TitleSkydome.h"
#include <memory>

using namespace KamataEngine;

class TitleScene : public IScene {
public:
	~TitleScene() override;
	void Initialize() override;
	void Update(float deltaTime) override;
	void Draw() override;
	bool IsFinished() const override { return isFinished_; }

	// Stateパターン用の状態遷移関数
	void ChangeState(std::unique_ptr<TitleFadeState> newState);

	// --- 状態クラスからアクセスするためのヘルパー関数群 ---
	Input* GetInput() const { return input_; }
	float GetFadeTimer() const { return fadeTimer_; }
	float GetFadeDuration() const { return fadeDuration_; }
	void ResetFadeTimer() { fadeTimer_ = 0.0f; }
	void AdvanceFadeTimer(float deltaTime) { fadeTimer_ += deltaTime; }
	void SetFadeAlpha(float alpha) { fadeAlpha_ = alpha; }
	void SetFinished(bool finished) { isFinished_ = finished; }

private:
	std::unique_ptr<TitleFadeState> fadeState_;

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	Camera camera_;

	// アセットモデルは非所有なので生ポインタでOKです
	Model* modelSkydome_ = nullptr;

	// ⭕ 指摘事項: TitleSkydome を std::unique_ptr に変更
	std::unique_ptr<TitleSkydome> titleSkydome_ = nullptr;

	uint32_t titleTextureHandle_ = 0;
	// ⭕ 指摘事項: スプライト群を std::unique_ptr に変更
	std::unique_ptr<Sprite> titleSprite_ = nullptr;

	uint32_t startTextureHandle_ = 0;
	std::unique_ptr<Sprite> startSprite_ = nullptr;

	std::unique_ptr<Sprite> fadeSprite_ = nullptr;

	Vector2 spritePos_ = {0.0f, 0.0f};
	Vector2 startPos_ = {0.0f, 0.0f};

	float totalTime_ = 0.0f;
	float fadeTimer_ = 0.0f;
	float fadeAlpha_ = 1.0f;
	float fadeDuration_ = 1.0f;
	float startAlpha_ = 1.0f;

	bool isFinished_ = false;
};