#include "DeathState.h"
#include "GameScene.h"

void DeathState::Enter(GameScene* scene) {
	// 死亡演出中は操作できないように入力を禁止する
	scene->SetPlayerInputEnabled(false);
}

void DeathState::Update(GameScene* scene, float deltaTime) {

	deltaTime;

	// 1. 死亡フェードイン処理
	float deathAlpha = scene->GetDeathAlpha();
	if (deathAlpha < 1.0f) {
		deathAlpha += 1.0f / 60.0f; // 約1秒で真っ暗に
		if (deathAlpha > 1.0f)
			deathAlpha = 1.0f;
		scene->SetDeathAlpha(deathAlpha);
	} else {
		// 2. 画面が真っ暗になったら、自動的にリトライ（ゲームシーン再生成）
		scene->ResetGame();
	}
}