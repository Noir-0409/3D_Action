#pragma once
#include "PlayerState.h"

namespace KamataEngine {

// 通常状態
class PlayerNormalState : public PlayerState {
public:
	void Update(Player* player) override;
};

// 死亡落下状態
class PlayerDeathFallState : public PlayerState {
public:
	void Update(Player* player) override;
};

} // namespace KamataEngine