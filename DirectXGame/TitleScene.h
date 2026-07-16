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

	void ChangeState(std::unique_ptr<TitleFadeState> newState);

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

	Model* modelSkydome_ = nullptr;

	std::unique_ptr<TitleSkydome> titleSkydome_ = nullptr;

	uint32_t titleTextureHandle_ = 0;
	
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