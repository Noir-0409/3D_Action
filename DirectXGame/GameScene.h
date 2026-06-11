#pragma once
#include "BaseBlock.h"
#include "CameraController.h"
#include "DeathParticle.h"
#include "Enemy.h"
#include "GameObject.h"
#include "GamePhaseState.h" // ★ 追加
#include "IScene.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MathUtillity.h"
#include "Player.h"
#include "Skydome.h"
#include <memory> // ★ 追加
#include <vector>
#include "EnemyFactory.h"
#include "CollisionObserver.h"

using namespace KamataEngine;

class GameScene : public IScene {
public:
	~GameScene() override;
	void Initialize() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void GenerateBlocks();
	bool IsFinished() const override { return isFinished_; }

	// ★ Stateパターン用のフェーズ遷移関数 (switch文の代わりにポインタを差し替える)
	void ChangePhase(std::unique_ptr<GamePhaseState> newPhase);

	// --- 各フェーズクラスから安全にデータを操作するためのヘルパー関数群 ★ ---
	Player* GetPlayer() const { return player_; }
	CameraController* GetCameraController() const { return cameraController_; }
	DeathParticle* GetDeathParticles() const { return deathParticles_; }
	Input* GetInput() const { return input_; }

	float GetCountdownTimer() const { return countdownTimer_; }
	void SetCountdownTimer(float time) { countdownTimer_ = time; }

	float GetStartAlpha() const { return startAlpha_; }
	void SetStartAlpha(float alpha) { startAlpha_ = alpha; }

	float GetFadeAlpha() const { return fadeAlpha_; }
	void SetFadeAlpha(float alpha) { fadeAlpha_ = alpha; }
	float GetFadeSpeed() const { return fadeSpeed_; }

	float GetOverAlpha() const { return overAlpha_; }
	void SetOverAlpha(float alpha) { overAlpha_ = alpha; }

	float GetClearAlpha() const { return clearAlpha_; }
	void SetClearAlpha(float alpha) { clearAlpha_ = alpha; }

	void SetFinished(bool finished) { isFinished_ = finished; }

	Sprite* GetStartSprite() const { return startSprite_; }
	Sprite* GetGuideSprite() const { return guideSprite_; }
	Sprite* GetOverSprite() const { return overSprite_; }
	Sprite* GetClearSprite() const { return clearSprite_; }

	// 共通で行うブロックの行列更新処理
	void UpdateBlocksMatrix() {
		for (auto& line : blocks_) {
			for (auto* block : line) {
				if (block) {
					WorldTransform* wt = block->GetWorldTransform();
					if (wt)
						wt->UpdateMatrix();
				}
			}
		}
	}

	// 死亡時のパーティクル初期化用ヘルパー
	void CreateDeathParticlesIfNeeded() {
		if (!deathParticles_) {
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			deathParticles_ = new DeathParticle();
			deathParticles_->Initialize(modelParticle_, &camera_, deathParticlesPosition);
		}
	}

	// ゴール時のパーティクル初期化用ヘルパー
	void CreateGoalParticlesIfNeeded() {
		if (!deathParticles_) {
			deathParticles_ = new DeathParticle();
			deathParticles_->Initialize(modelParticle_, &camera_, player_->GetWorldPosition());
		}
	}

	void NotifyCollisions();

private:
	
	std::vector<std::unique_ptr<CollisionObserver>> collisionObservers_;
	std::unique_ptr<EnemyFactory> enemyFactory_;

	// ★ 現在のフェーズ状態を管理するスマートポインタ
	std::unique_ptr<GamePhaseState> phaseState_;

	Camera camera_;
	WorldTransform worldTransform_;
	Input* input_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Player* player_ = nullptr;
	Model* modelBlock_ = nullptr;
	Model* modelFire_ = nullptr;
	Model* modelGoal_ = nullptr;
	Model* modelIce_ = nullptr;
	Model* modelRed_ = nullptr;
	Model* modelBlue_ = nullptr;
	Model* modelRed2_ = nullptr;
	Model* modelBlue2_ = nullptr;
	std::vector<std::vector<BaseBlock*>> blocks_;
	MapChipField* mapChipField_;
	CameraController* cameraController_;
	std::vector<GameObject*> gameObjects_;
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
	uint32_t guideTextureHandle_ = 0;
	Sprite* guideSprite_ = nullptr;

	Vector2 numberPos_ = {0, 0};
	Vector2 startPos_ = {0, 0};
	Vector2 overPos_ = {0, 0};
	Vector2 titlePos = {0, 0};
	Vector2 guidePos_ = {0, 0};

	float countdownScale_ = 1.0f;
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
	Sprite* fadeSprite_ = nullptr;
	float fadeAlpha_ = 0.0f;
	float fadeSpeed_ = 1.0f / 120.0f;
};