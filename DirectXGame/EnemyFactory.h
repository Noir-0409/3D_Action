#pragma once
#include "Enemy.h"
#include "KamataEngine.h"

class EnemyFactory {
public:

	static Enemy* CreateEnemy(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
};