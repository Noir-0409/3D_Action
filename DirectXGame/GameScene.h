#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "MapChipField.h"
#include "CameraController.h"
#include "Enemy.h"
#include "Skydome.h"
#include "MathUtillity.h"
#include "DeathParticle.h"

using namespace KamataEngine;

// ゲームシーン
class GameScene {

	enum class Phase {

		kCountDown,
		kPlay,
		kDeath,

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

	Model* modelBlock_ = nullptr;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	MapChipField* mapChipField_;

	CameraController* cameraController_;

	//Enemy* enemy_ = nullptr;
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

	Vector2 numberPos_ = {0, 0};
	Vector2 startPos_ = {0, 0};

};