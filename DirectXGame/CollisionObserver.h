#pragma once
#include <memory>

class Player;
class Enemy;

// インターフェース
class CollisionObserver {
public:
	virtual ~CollisionObserver() = default;
	virtual void OnPlayerEnemyCollision(Player* player, Enemy* enemy) = 0;
};

// 具象クラス（中身をcppに引っ越すため、ここでは宣言だけ）
class PlayerEnemyCollisionHandler : public CollisionObserver {
public:
	void OnPlayerEnemyCollision(Player* player, Enemy* enemy) override;
};