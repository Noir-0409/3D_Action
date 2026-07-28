#include "CollisionObserver.h"
#include "Enemy.h"
#include "Player.h"

namespace KamataEngine { // ★ 追加

void PlayerEnemyCollisionHandler::OnPlayerEnemyCollision(Player* player, Enemy* enemy) {
	if (player && enemy) {
		player->OnCollision(enemy);
		enemy->OnCollision(player);
	}
}

} // namespace KamataEngine