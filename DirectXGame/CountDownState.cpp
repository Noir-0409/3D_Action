#include "CountDownState.h"
#include "GameScene.h"

void CountDownState::Enter(GameScene* scene) {
	// 状態に入ったとき、プレイヤーの入力をあらかじめ無効化しておく
	scene->SetPlayerInputEnabled(false);
}

void CountDownState::Update(GameScene* scene, float deltaTime) {
	// 1. カウントダウンタイマーを減らす
	float timer = scene->GetCountdownTimer();
	timer -= deltaTime;
	scene->SetCountdownTimer(timer);

	// 2. 開始スプライトのフェードイン処理
	float startAlpha = scene->GetStartAlpha();
	if (startAlpha < 1.0f) {
		startAlpha += 1.0f / 120.0f; // 約2秒でフル表示
		if (startAlpha > 1.0f) {
			startAlpha = 1.0f;
		}
		scene->SetStartAlpha(startAlpha);
	}

	// 3. タイマーがゼロ以下になったら「プレイ状態」へ切り替える
	if (timer <= 0.0f) {
		// ★GameSceneに新しく作る状態遷移用の関数を呼び出す（後ほどGameScene側を修正します）
		scene->ChangeStateToPlay();
	}
}