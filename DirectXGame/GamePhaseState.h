#pragma once

namespace KamataEngine { // ★ namespace の開始

class GameScene;

class GamePhaseState {
public:
	virtual ~GamePhaseState() = default;
	virtual void Update(GameScene* scene) = 0;
	virtual void Draw(GameScene* scene) = 0;
};

} // namespace KamataEngine
