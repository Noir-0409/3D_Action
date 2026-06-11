#pragma once
#include "IGameState.h"

class CountDownState : public IGameState {
public:
	// 状態に入った瞬間（初期化）
	void Enter(GameScene* scene) override;

	// 毎フレームの更新処理
	void Update(GameScene* scene, float deltaTime) override;
};