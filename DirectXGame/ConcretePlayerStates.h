#pragma once
#include "PlayerState.h"

// 通常状態（移動やジャンプ、衝突判定などを行う）
class PlayerNormalState : public PlayerState {
public:
	void Update(Player* player) override;
};

// 死亡落下状態（ミスして画面外に落ちていく演出）
class PlayerDeathFallState : public PlayerState {
public:
	void Update(Player* player) override;
};