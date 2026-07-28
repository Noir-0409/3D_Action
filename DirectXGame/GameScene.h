#pragma once
#include "BaseBlock.h"
#include "CameraController.h"
#include "CollisionObserver.h"
#include "DeathParticle.h"
#include "Enemy.h"
#include "EnemyFactory.h"
#include "GameObject.h"
#include "GamePhaseState.h"
#include "IScene.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MathUtillity.h"
#include "Player.h"
#include "Skydome.h"
#include <list>
#include <memory>
#include <vector>

// ★ ヘッダーファイルでの using namespace KamataEngine; は削除します（名前空間汚染防止のため）

namespace KamataEngine { // ★ namespace の開始

class GameScene : public IScene {
public:
	~GameScene() override;
	void Initialize() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void GenerateBlocks();
	bool IsFinished() const override { return isFinished_; }

	void ChangePhase(std::unique_ptr<GamePhaseState> newPhase);

	// --- ゲッターの戻り値をスマートポインタの実態に合わせて変更（.get()で生のポインタを渡すようにします） ---
	Player* GetPlayer() const { return player_.get(); }
	CameraController* GetCameraController() const { return cameraController_.get(); }
	DeathParticle* GetDeathParticles() const { return deathParticles_.get(); }
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

	Sprite* GetStartSprite() const { return startSprite_.get(); }
	Sprite* GetGuideSprite() const { return guideSprite_.get(); }
	Sprite* GetOverSprite() const { return overSprite_.get(); }
	Sprite* GetClearSprite() const { return clearSprite_.get(); }

	void UpdateBlocksMatrix() {
		for (auto& line : blocks_) {
			for (auto& block : line) { // ★ ポインタではなくスマートポインタの参照に変更
				if (block) {
					WorldTransform* wt = block->GetWorldTransform();
					if (wt)
						wt->UpdateMatrix();
				}
			}
		}
	}

	void CreateDeathParticlesIfNeeded() {
		if (!deathParticles_) {
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			deathParticles_ = std::make_unique<DeathParticle>(); // ★ std::make_uniqueを使用
			deathParticles_->Initialize(modelParticle_, &camera_, deathParticlesPosition);
		}
	}

	void CreateGoalParticlesIfNeeded() {
		if (!deathParticles_) {
			deathParticles_ = std::make_unique<DeathParticle>(); // ★ std::make_uniqueを使用
			deathParticles_->Initialize(modelParticle_, &camera_, player_->GetWorldPosition());
		}
	}

	void NotifyCollisions();

private:
	std::vector<std::unique_ptr<CollisionObserver>> collisionObservers_;
	std::unique_ptr<EnemyFactory> enemyFactory_;
	std::unique_ptr<GamePhaseState> phaseState_;

	Camera camera_;
	WorldTransform worldTransform_;
	Input* input_ = nullptr;

	// アセット（モデル）はエンジン管理、またはGameSceneが所有権を持たないので生ポインタのままでOK
	Model* modelPlayer_ = nullptr;
	Model* modelBlock_ = nullptr;
	Model* modelFire_ = nullptr;
	Model* modelGoal_ = nullptr;
	Model* modelIce_ = nullptr;
	Model* modelRed_ = nullptr;
	Model* modelBlue_ = nullptr;
	Model* modelRed2_ = nullptr;
	Model* modelBlue2_ = nullptr;
	Model* modelEnemy_ = nullptr;
	Model* modelSkydome_ = nullptr;
	Model* modelParticle_ = nullptr;

	// ❌ 生ポインタの配列から、⭕ スマートポインタの配列へ変更！
	std::vector<std::vector<std::unique_ptr<BaseBlock>>> blocks_;
	std::unique_ptr<MapChipField> mapChipField_;
	std::unique_ptr<CameraController> cameraController_;

	std::vector<std::unique_ptr<Enemy>> enemyOwnerList_;

	// gameObjects_ 自体に寿命を管理させ、各具体的なスマートポインタから非所有ポインタ（.get()）を登録する形にします
	std::vector<GameObject*> gameObjects_;
	std::list<Enemy*> enemies_; // 判定用の非所有リスト

	std::unique_ptr<Player> player_;
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<DeathParticle> deathParticles_;

	bool isFinished_ = false;

	float countdownTimer_ = 0.0f;
	int countdownNumber_ = 3;

	uint32_t oneTextureHandle_ = 0;
	std::unique_ptr<Sprite> oneSprite_ = nullptr;
	uint32_t twoTextureHandle_ = 0;
	std::unique_ptr<Sprite> twoSprite_ = nullptr;
	uint32_t threeTextureHandle_ = 0;
	std::unique_ptr<Sprite> threeSprite_ = nullptr;
	uint32_t startTextureHandle_ = 0;
	std::unique_ptr<Sprite> startSprite_ = nullptr;
	uint32_t clearTextureHandle_ = 0u;
	std::unique_ptr<Sprite> clearSprite_ = nullptr;
	uint32_t overTextureHandle_ = 0;
	std::unique_ptr<Sprite> overSprite_ = nullptr;
	uint32_t overTitleTextureHandle_ = 0;
	std::unique_ptr<Sprite> overTitleSprite_ = nullptr;
	uint32_t guideTextureHandle_ = 0;
	std::unique_ptr<Sprite> guideSprite_ = nullptr;

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
	std::unique_ptr<Sprite> fadeSprite_ = nullptr;
	float fadeAlpha_ = 0.0f;
	float fadeSpeed_ = 1.0f / 120.0f;
};

} // namespace KamataEngine