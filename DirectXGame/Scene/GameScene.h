#pragma once
#include "CameraController.h"
#include "DeathParticle.h"
#include "Enemy.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MathUtillity.h"
#include "Player.h"
#include "Skydome.h"

using namespace KamataEngine;

// ゲームシーン
class GameScene {

	enum class Phase {

		kCountDown,
		kPlay,
		kDeath,
		kGoal

	};

	Phase phase_;

public:
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	void GenerateBlocks();

	bool IsFinished() { return isFinished_; }

	void CheckAllCollision();

	void ChangePhase();

private:
	// カメラ
	Camera camera_;

	WorldTransform worldTransform_;

	Input* input_ = nullptr;

	Model* modelPlayer_ = nullptr;

	Player* player_ = nullptr;

	// 1のブロック
	Model* modelBlock_ = nullptr;

	// 2の即死ブロック
	Model* modelFire_ = nullptr;

	Model* modelGoal_ = nullptr;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	MapChipField* mapChipField_;

	CameraController* cameraController_;

	// Enemy* enemy_ = nullptr;
	std::list<Enemy*> enemies_;

	Model* modelEnemy_ = nullptr;

	Skydome* skydome_ = nullptr;

	Model* modelSkydome_ = nullptr;

	bool isFinished_ = false;

	DeathParticle* deathParticles_ = nullptr;

	Model* modelParticle_ = nullptr;

	float countdownTimer_ = 0.0f;
	int countdownNumber_ = 3;

	uint32_t oneTextureHandle_ = 0;
	Sprite* oneSprite_ = nullptr;

	uint32_t twoTextureHandle_ = 0;
	Sprite* twoSprite_ = nullptr;

	uint32_t threeTextureHandle_ = 0;
	Sprite* threeSprite_ = nullptr;

	uint32_t startTextureHandle_ = 0;
	Sprite* startSprite_ = nullptr;

	uint32_t clearTextureHandle_ = 0u;
	Sprite* clearSprite_ = nullptr;

	uint32_t overTextureHandle_ = 0;
	Sprite* overSprite_ = nullptr;

	uint32_t overTitleTextureHandle_ = 0;
	Sprite* overTitleSprite_ = nullptr;

	Vector2 numberPos_ = {0, 0};
	Vector2 startPos_ = {0, 0};
	Vector2 overPos_ = {0, 0};
	Vector2 titlePos = {0, 0};

	float countdownScale_ = 1.0f; // スプライトの拡大率
	float countdownScaleSpeed_ = 0.05f;

	uint32_t fireTextureHandle1_ = 0;
	uint32_t fireTextureHandle2_ = 0;
	float fireSwitchTimer_ = 0.0f;
	float fireSwitchInterval_ = 1.0f;
	bool fireToggle_ = false;

	float startAlpha_ = 0.0f;
	float overAlpha_ = 0.0f;
	float clearAlpha_;

	uint32_t fadeTextureHandle_ = 0;
	Sprite* fadeSprite_ = nullptr; // 死亡フェード用スプライト
	float fadeAlpha_ = 0.0f;       // 0.0 = 透明, 1.0 = 真っ黒
	float fadeSpeed_ = 1.0f / 120.0f;

};