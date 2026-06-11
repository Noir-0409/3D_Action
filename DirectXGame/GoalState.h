#include "IGameState.h"

class GoalState : public IGameState {
public:
	void Enter(GameScene* scene) override;
	void Update(GameScene* scene, float deltaTime) override;
};