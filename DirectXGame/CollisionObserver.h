#pragma once

class Player;
class Enemy;

class CollisionObserver {
public:
	virtual ~CollisionObserver() = default;

	virtual void OnPlayerEnemyCollision(Player* player, Enemy* enemy) = 0;
};