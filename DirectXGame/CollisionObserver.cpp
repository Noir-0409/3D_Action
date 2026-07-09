#include "CollisionObserver.h"
#include "Enemy.h" 
#include "Player.h" 

void PlayerEnemyCollisionHandler::OnPlayerEnemyCollision(Player* player, Enemy* enemy) {
	if (player && enemy) {
		player->OnCollision(enemy);
		enemy->OnCollision(player);
	}
}