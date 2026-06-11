#include "GameScene.h"
#include "PlayerState.h"

void PlayState::Enter(GameScene* scene) {
	// プレイ状態に入ったら、プレイヤーの入力を有効化する
	scene->SetPlayerInputEnabled(true);
}

void PlayState::Update(GameScene* scene, float deltaTime) {
	// 1. ゲーム内タイマーを更新する（増やす）
	float gameTimer = scene->GetGameTimer();
	gameTimer += deltaTime;
	scene->SetGameTimer(gameTimer);

	// 2. プレイヤーが死亡したか（穴に落ちた、敵に当たったなど）をGameSceneにチェックしてもらう
	if (scene->CheckPlayerDeath()) {
		scene->ChangeStateToDeath(); // 死亡状態へ切り替え
		return;
	}

	// 3. プレイヤーがゴールしたかをGameSceneにチェックしてもらう
	if (scene->CheckPlayerGoal()) {
		scene->ChangeStateToGoal(); // ゴール状態へ切り替え
		return;
	}
}