#pragma once
#include "TitleFadeState.h"

namespace KamataEngine { // ★ namespace の開始

class TitleScene; // ★ 前方宣言

// フェードイン状態
class TitleFadeInState : public TitleFadeState {
public:
	void Update(TitleScene* scene, float deltaTime) override;
};

// 待機状態（通常画面・入力受付）
class TitleNoneState : public TitleFadeState {
public:
	void Update(TitleScene* scene, float deltaTime) override;
};

// フェードアウト状態
class TitleFadeOutState : public TitleFadeState {
public:
	void Update(TitleScene* scene, float deltaTime) override;
};

} // namespace KamataEngine