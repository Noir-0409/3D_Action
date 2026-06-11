#include "GoalState.h"
#include "GameScene.h"

void GoalState::Enter(GameScene* scene) {
	// ゴール演出中も操作できないように入力を禁止する
	scene->SetPlayerInputEnabled(false);
}

void GoalState::Update(GameScene* scene, float deltaTime) {

	deltaTime;

	// 1. ゴール（クリア）スプライトのフェードイン処理
	float goalAlpha = scene->GetGoalAlpha();
	if (goalAlpha < 1.0f) {
		goalAlpha += 1.0f / 60.0f;
		if (goalAlpha > 1.0f)
			goalAlpha = 1.0f;
		scene->SetGoalAlpha(goalAlpha);
	}

	// 2. スペースキーが押されたら、タイトル画面へ戻る処理をGameSceneに頼む
	if (scene->IsSpaceKeyPressed()) {
		scene->BackToTitle();
	}
}