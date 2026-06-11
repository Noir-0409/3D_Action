#pragma once
#include "CameraController.h"
#include "Enemy.h"
#include "3D/Model.h"
#include <memory>

// 工場のインターフェース（抽象クラス）
class EnemyFactory {
public:
	virtual ~EnemyFactory() = default;

	// 敵を生成して初期化まで行う純粋仮想関数
	virtual std::unique_ptr<Enemy> CreateEnemy(Model* model, Camera* camera, const Vector3& position) = 0;
};

// 実際の製造を行う具体的な工場（具象クラス）
class ConcreteEnemyFactory : public EnemyFactory {
public:
	std::unique_ptr<Enemy> CreateEnemy(Model* model, Camera* camera, const Vector3& position) override {
		// ここで new を行う（GameSceneの代わりに new してあげる）
		auto enemy = std::make_unique<Enemy>();
		enemy->Initialize(model, camera, position);
		return enemy;
	}
};