#pragma once
#include "CameraController.h"
#include "Enemy.h"
#include "3D/Model.h"
#include "Player.h"
#include "Skydome.h"
#include <memory>

// GameObjectを生成するための工場クラス
class GameObjectFactory {
public:
	// スカイドームの生成
	static Skydome* CreateSkydome(Model* model, Camera* camera) {
		Skydome* skydome = new Skydome();
		skydome->Initialize(model, camera);
		return skydome;
	}

	// プレイヤーの生成
	static Player* CreatePlayer(Model* model, Camera* camera, const Vector3& position, MapChipField* mapChipField) {
		Player* player = new Player();
		player->Initialize(model, camera, position);
		player->SetMapChipField(mapChipField);
		return player;
	}

	// 敵の生成（★指摘された一番重要な部分！）
	static Enemy* CreateEnemy(Model* model, Camera* camera, const Vector3& position) {
		Enemy* enemy = new Enemy();
		enemy->Initialize(model, camera, position);
		return enemy;
	}
};