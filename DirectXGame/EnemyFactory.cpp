#include "EnemyFactory.h"

Enemy* EnemyFactory::CreateEnemy(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// GameSceneで行っていた new と Initialize をここで肩代わりする
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(model, camera, position);

	return newEnemy;
}