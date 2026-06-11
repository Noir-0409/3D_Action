#pragma once
#include "GamePhaseState.h"

// カウントダウンフェーズ
class CountDownPhase : public GamePhaseState {
public:
	void Update(GameScene* scene) override;
	void Draw(GameScene* scene) override;
};

// プレイフェーズ
class PlayPhase : public GamePhaseState {
public:
	void Update(GameScene* scene) override;
	void Draw(GameScene* scene) override;
};

// 死亡（ゲームオーバー）フェーズ
class DeathPhase : public GamePhaseState {
public:
	void Update(GameScene* scene) override;
	void Draw(GameScene* scene) override;
};

// ゴール（クリア）フェーズ
class GoalPhase : public GamePhaseState {
public:
	void Update(GameScene* scene) override;
	void Draw(GameScene* scene) override;
};