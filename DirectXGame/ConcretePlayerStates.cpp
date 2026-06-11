#include "ConcretePlayerStates.h"
#include "Player.h"
#include <algorithm>

// --- 通常状態の処理 ---
void PlayerNormalState::Update(Player* player) {
	// 1. 入力による移動速度計算
	if (player->inputEnabled_) {
		player->InputMove();
	} else {
		player->SetVelocity({0.0f, 0.0f, 0.0f});
	}

	// 2. マップ衝突判定と移動の反映
	Player::CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = player->GetVelocity();

	player->CheckMapCollision(collisionMapInfo);
	player->CollisionMove(collisionMapInfo);
	player->UpdateOnGround(collisionMapInfo);
	player->UpdateHitWall(collisionMapInfo);

	// 3. 攻撃タイマー等の更新
	if (player->IsAttacking()) {
		player->DecrementAttackTimer();
	}

	player->IsOnIce();
}

// --- 死亡落下状態の処理 ---
void PlayerDeathFallState::Update(Player* player) {
	// Y方向の速度に重力を加算
	player->ApplyDeathGravity();

	// Z軸回転 (60FPS想定)
	player->UpdateDeathRotation(1.0f / 60.0f);

	// 画面下まで落ちたら、通常状態（あるいは停止など）に戻る、または何もしない
	if (player->GetWorldPosition().y < -50.0f) {
		// 必要であればここで状態を戻す、あるいは終了フラグを立てる
		player->StopDeathFall();
	}
}