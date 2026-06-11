#pragma once

class GameScene;

class IGameState {

public:

	virtual ~IGameState() = default;

	virtual void Enter(GameScene* scene) = 0;

	virtual void Update(GameScene* scene, float deltaTime) = 0;

	virtual void Draw(GameScene* scene) { (void)scene; }
};