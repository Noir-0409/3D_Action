#define NOMINMAX

#include "ConcreteGamePhases.h"
#include "GameScene.h"

// ==========================================
// 【1】カウントダウンフェーズ
// ==========================================
void CountDownPhase::Update(GameScene* scene) {
	scene->SetCountdownTimer(scene->GetCountdownTimer() - (1.0f / 60.0f));
	if (scene->GetCountdownTimer() <= 0.0f) {
		// カウントダウン終了でプレイフェーズへ
		scene->ChangePhase(std::make_unique<PlayPhase>());
		return;
	}

	if (scene->GetStartAlpha() < 1.0f) {
		scene->SetStartAlpha(std::min(1.0f, scene->GetStartAlpha() + (1.0f / 120.0f)));
	}

	scene->GetPlayer()->SetInputEnabled(false);
	scene->UpdateBlocksMatrix();
	scene->GetCameraController()->Update();
	if (scene->GetDeathParticles()) {
		scene->GetDeathParticles()->Update();
	}
}

void CountDownPhase::Draw(GameScene* scene) {
	if (scene->GetStartSprite()) {
		scene->GetStartSprite()->SetColor({1.0f, 1.0f, 1.0f, scene->GetStartAlpha()});
		scene->GetStartSprite()->Draw();
	}
}

// ==========================================
// 【2】プレイフェーズ
// ==========================================
void PlayPhase::Update(GameScene* scene) {
	scene->GetPlayer()->SetInputEnabled(true);

	// 当たり判定チェック
	scene->CheckAllCollision();
	scene->UpdateBlocksMatrix();

	if (scene->GetDeathParticles()) {
		scene->GetDeathParticles()->Update();
	}

	scene->GetCameraController()->Update();

	// 状態遷移のチェック（クラスの内部で制御）
	if (scene->GetPlayer()->IsDead()) {
		scene->ChangePhase(std::make_unique<DeathPhase>());
	} else if (scene->GetPlayer()->IsGoal()) {
		scene->ChangePhase(std::make_unique<GoalPhase>());
	}
}

void PlayPhase::Draw(GameScene* scene) {
	if (scene->GetGuideSprite()) {
		scene->GetGuideSprite()->Draw();
	}
}

// ==========================================
// 【3】死亡フェーズ
// ==========================================
void DeathPhase::Update(GameScene* scene) {
	// 遷移の初回時のみパーティクルを生成するヘルパーを呼ぶ
	scene->CreateDeathParticlesIfNeeded();

	scene->UpdateBlocksMatrix();

	if (scene->GetFadeAlpha() < 1.0f) {
		scene->SetFadeAlpha(std::min(1.0f, scene->GetFadeAlpha() + scene->GetFadeSpeed()));
	}

	if (scene->GetOverAlpha() < 1.0f) {
		scene->SetOverAlpha(std::min(1.0f, scene->GetOverAlpha() + (1.0f / 180.0f)));
	}

	if (scene->GetInput()->TriggerKey(DIK_SPACE)) {
		scene->SetFinished(true);
	}

	if (scene->GetDeathParticles()) {
		scene->GetDeathParticles()->Update();
	}

	scene->GetCameraController()->Update();
}

void DeathPhase::Draw(GameScene* scene) {
	if (scene->GetOverSprite()) {
		scene->GetOverSprite()->SetColor({1.0f, 1.0f, 1.0f, scene->GetOverAlpha()});
		scene->GetOverSprite()->Draw();
	}
}

// ==========================================
// 【4】ゴールフェーズ
// ==========================================
void GoalPhase::Update(GameScene* scene) {
	scene->GetPlayer()->SetInputEnabled(false);

	// ゴール演出用のパーティクル生成
	scene->CreateGoalParticlesIfNeeded();

	if (scene->GetDeathParticles()) {
		scene->GetDeathParticles()->Update();
	}

	if (scene->GetFadeAlpha() < 1.0f) {
		scene->SetFadeAlpha(std::min(1.0f, scene->GetFadeAlpha() + scene->GetFadeSpeed()));
	}

	if (scene->GetFadeAlpha() >= 1.0f) {
		if (scene->GetClearAlpha() < 1.0f) {
			scene->SetClearAlpha(std::min(1.0f, scene->GetClearAlpha() + (1.0f / 180.0f)));
		}
	}

	scene->UpdateBlocksMatrix();
	scene->GetCameraController()->Update();

	if (scene->GetInput()->TriggerKey(DIK_SPACE)) {
		scene->SetFinished(true);
	}
}

void GoalPhase::Draw(GameScene* scene) {
	if (scene->GetClearSprite()) {
		scene->GetClearSprite()->SetColor({1.0f, 1.0f, 1.0f, scene->GetClearAlpha()});
		scene->GetClearSprite()->Draw();
	}
}