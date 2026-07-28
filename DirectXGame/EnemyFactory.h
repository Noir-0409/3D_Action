#pragma once
#include "3D/Model.h"
#include "CameraController.h"
#include "Enemy.h"
#include <memory>

namespace KamataEngine { // ★ namespace の開始

// 工場のインターフェース（抽象クラス）
class EnemyFactory {
public:
	virtual ~EnemyFactory() = default;

	// 敵を生成して初期化まで行う純粋仮想関数
	// ※ Enemy::Initialize に合わせて const Camera* に変更しています
	virtual std::unique_ptr<Enemy> CreateEnemy(Model* model, const Camera* camera, const Vector3& position) = 0;
};

// 実際の製造を行う具体的な工場（具象クラス）
class ConcreteEnemyFactory : public EnemyFactory {
public:
	std::unique_ptr<Enemy> CreateEnemy(Model* model, const Camera* camera, const Vector3& position) override {
		// ここで new を行う（GameSceneの代わりに new してあげる）
		auto enemy = std::make_unique<Enemy>();
		enemy->Initialize(model, camera, position);
		return enemy;
	}
};

} // namespace KamataEngine