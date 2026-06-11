#pragma once
#include "GameObject.h"
#include "IGameState.h" // ★追加
#include "IScene.h"
#include <vector>

// 前置宣言
class Player;

/// <summary>
/// ゲーム本編のシーン管理クラス
/// </summary>
class GameScene : public IScene {
public:
	GameScene();
	~GameScene();

	void Initialize() override;
	void Update(float deltaTime) override;
	void Draw() override;
	bool IsFinished() const override;

	void ChangeState(IGameState* newState);
	void ChangeStateToPlay();
	void ChangeStateToDeath();
	void ChangeStateToGoal();

	// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	// ★ステートパターン用：各状態クラスがデータにアクセスするための関数
	// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	void SetPlayerInputEnabled(bool enabled);

	float GetCountdownTimer() const { return countdownTimer_; }
	void SetCountdownTimer(float time) { countdownTimer_ = time; }

	float GetGameTimer() const { return gameTimer_; }
	void SetGameTimer(float time) { gameTimer_ = time; }

	float GetStartAlpha() const { return startAlpha_; }
	void SetStartAlpha(float alpha) { startAlpha_ = alpha; }

	float GetDeathAlpha() const { return deathAlpha_; }
	void SetDeathAlpha(float alpha) { deathAlpha_ = alpha; }

	float GetGoalAlpha() const { return goalAlpha_; }
	void SetGoalAlpha(float alpha) { goalAlpha_ = alpha; }

	bool CheckPlayerDeath();
	bool CheckPlayerGoal();
	bool IsSpaceKeyPressed();

	void ResetGame();
	void BackToTitle();

private:

	Camera* camera_;

	Model* modelSkydome_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* modelEnemy_ = nullptr;

	// ポリモーフィズムで管理されるオブジェクトたち
	std::vector<GameObject*> gameObjects_;
	Player* player_ = nullptr; // プレイヤー固有の処理（フラグ確認など）を呼ぶため、ポインタは残す

	class MapChipField* mapChipField_ = nullptr;

	// ★ステートパターン：現在の方針（状態）を指すポインタ
	IGameState* currentState_ = nullptr;

	// ゲーム内で使う様々な変数（以前のものと名前を合わせています）
	float countdownTimer_ = 3.0f;
	float gameTimer_ = 0.0f;
	float startAlpha_ = 0.0f;
	float deathAlpha_ = 0.0f;
	float goalAlpha_ = 0.0f;

	// シーン遷移用のフラグなど（あなたの環境に合わせて調整してください）
	bool isFinished_ = false;
};