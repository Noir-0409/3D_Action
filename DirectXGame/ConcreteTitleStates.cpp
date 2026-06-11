#include "ConcreteTitleStates.h"
#include "TitleScene.h"

// --- フェードイン状態 ---
void TitleFadeInState::Update(TitleScene* scene, float deltaTime) {
	scene->AdvanceFadeTimer(deltaTime);

	// アルファ値を減少させる（だんだん明るく）
	float alpha = 1.0f - (scene->GetFadeTimer() / scene->GetFadeDuration());
	if (alpha <= 0.0f) {
		alpha = 0.0f;
		scene->SetFadeAlpha(alpha);
		// フェードインが終わったら「待機状態」へ遷移
		scene->ChangeState(std::make_unique<TitleNoneState>());
	} else {
		scene->SetFadeAlpha(alpha);
	}
}

// --- 待機状態（入力待ち） ---
void TitleNoneState::Update(TitleScene* scene, float deltaTime) {
	(void)deltaTime;
	// スペースキーが押されたらフェードアウトへ遷移
	if (scene->GetInput()->TriggerKey(DIK_SPACE)) {
		scene->ResetFadeTimer();
		scene->ChangeState(std::make_unique<TitleFadeOutState>());
	}
}

// --- フェードアウト状態 ---
void TitleFadeOutState::Update(TitleScene* scene, float deltaTime) {
	scene->AdvanceFadeTimer(deltaTime);

	// アルファ値を増加させる（だんだん暗く）
	float alpha = scene->GetFadeTimer() / scene->GetFadeDuration();
	if (alpha >= 1.0f) {
		alpha = 1.0f;
		scene->SetFadeAlpha(alpha);
		// フェードアウトが完了したらシーン終了フラグを立てる
		scene->SetFinished(true);
	} else {
		scene->SetFadeAlpha(alpha);
	}
}