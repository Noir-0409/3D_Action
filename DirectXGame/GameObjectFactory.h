#pragma once
#include "3D/Model.h"
#include "CameraController.h"
#include "Enemy.h"
#include "Player.h"
#include "Skydome.h"
#include <memory>

// GameObjectを生成するための工場クラス 
class GameObjectFactory {
public:
	// スカイドームの生成
	static std::unique_ptr<Skydome> CreateSkydome(Model* model, Camera* camera) {
		auto skydome = std::make_unique<Skydome>();
		skydome->Initialize(model, camera);
		return skydome;
	}

	// プレイヤーの生成
	static std::unique_ptr<Player> CreatePlayer(Model* model, Camera* camera, const Vector3& position, MapChipField* mapChipField) {
		auto player = std::make_unique<Player>();
		player->Initialize(model, camera, position);
		player->SetMapChipField(mapChipField);
		return player;
	}

	// 敵の生成
	static std::unique_ptr<Enemy> CreateEnemy(Model* model, Camera* camera, const Vector3& position) {
		auto enemy = std::make_unique<Enemy>();
		enemy->Initialize(model, camera, position);
		return enemy;
	}
};