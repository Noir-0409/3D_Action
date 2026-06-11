#pragma once
#include <memory>

// クラスの前方宣言（ヘッダーの循環インクルードを防ぐため）
class Player;
class Enemy;

// =========================================================================
// ✨ 【Observerパターン】通知を受け取る側のインターフェース（抽象クラス）
// =========================================================================
class CollisionObserver {
public:
	virtual ~CollisionObserver() = default;

	// プレイヤーと敵が衝突したときに呼ばれるイベント通知関数
	virtual void OnPlayerEnemyCollision(Player* player, Enemy* enemy) = 0;
};

// =========================================================================
// ✨ 【Observerパターン】実際に衝突が起きたときの応答処理を行う具象クラス
// =========================================================================
class PlayerEnemyCollisionHandler : public CollisionObserver {
public:
	void OnPlayerEnemyCollision(Player* player, Enemy* enemy) override {
		// GameSceneの代わりに、オブジェクト同士の衝突後処理を呼び出す
		if (player && enemy) {
			player->OnCollision(enemy);
			enemy->OnCollision(player);
		}
	}
};