#pragma once
#include "GamePhaseState.h"

namespace KamataEngine { // ★ namespace の開始

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

} // namespace KamataEngine