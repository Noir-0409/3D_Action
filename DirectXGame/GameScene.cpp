#include "GameScene.h"

GameScene::~GameScene() {

	delete modelPlayer_;
	delete player_;
	delete mapChipField_;
	delete modelParticle_;

	for (Enemy* enemy : enemies_) {

		delete enemy;
	}

	enemies_.clear();

	delete modelSkydome_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {

	modelPlayer_ = Model::CreateFromOBJ("Player");
	modelBlock_ = Model::CreateFromOBJ("block");
	modelEnemy_ = Model::CreateFromOBJ("enemy");
	modelSkydome_ = Model::CreateFromOBJ("skydome");
	modelParticle_ = Model::CreateFromOBJ("deathParticle"); 

	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCSV("Resources/map.csv");

	camera_.Initialize();

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	Vector3 basePosition = {25.0f, 1.0f, 0.0f}; // 基準となる位置
	Vector3 offset = {3.0f, 3.0f, 0.0f};        // 各敵の間隔

	for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();

		// 一体ずつ違う座標をセット
		Vector3 enemyPosition = basePosition + offset * static_cast<float>(i);

		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);

		enemies_.push_back(newEnemy);
	}

	cameraController_ = new CameraController();
	cameraController_->SetCamera(&camera_);
	cameraController_->SetTarget(player_);
	cameraController_->Initialize();
	cameraController_->Reset();

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	deathParticles_ = new DeathParticle();
	deathParticles_->Initialize(modelParticle_, &camera_, playerPosition);

	input_ = Input::GetInstance();

	worldTransform_.Initialize();

	GenerateBlocks();
}

void GameScene::Update() {

	 if (input_->TriggerKey(DIK_Z)) {
		player_->Attack();
	}

	  if (player_->IsAttacking()) {

		AABB attackBox = player_->GetAttackAABB();

		for (Enemy* enemy : enemies_) {

			AABB enemyBox = enemy->GetAABB();

			if (AABB::IsCollision(attackBox, enemyBox)) {
				// 敵にダメージなどの処理
				enemy->OnCollision(player_);
			}
		}
	}


	CheckAllCollision();

	player_->Update();
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			// アフィン変換行列の作成
			worldTransformBlockYoko->UpdateMatrix();
		}
	}

	if (input_->TriggerKey(DIK_SPACE)) {

		isFinished_ = true;
	}

	if (deathParticles_) {

		deathParticles_->Update();
	}

	cameraController_->Update();
	skydome_->Update();
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());

	Sprite::PostDraw();

	dxCommon->ClearDepthBuffer();

	Model::PreDraw();

	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			modelBlock_->Draw(*worldTransformBlockYoko, camera_);
		}
	}

	player_->Draw(camera_);
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	if (deathParticles_) {

		deathParticles_->Draw();
	}

	skydome_->Draw();

	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());

	Sprite::PostDraw();
}

void GameScene::GenerateBlocks() {

	// uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	// uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(MapChipField::kNumBlockVirtical);

	for (uint32_t i = 0; i < MapChipField::kNumBlockVirtical; ++i) {

		worldTransformBlocks_[i].resize(MapChipField::kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < MapChipField::kNumBlockVirtical; ++i) {

		worldTransformBlocks_[i].resize(MapChipField::kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < MapChipField::kNumBlockVirtical; ++i) {

		for (uint32_t j = 0; j < MapChipField::kNumBlockHorizontal; ++j) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {

				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::CheckAllCollision() {

	AABB aabb1, aabb2;

	aabb1 = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
	
		aabb2 = enemy->GetAABB();

		if (AABB::IsCollision(aabb1, aabb2)) {

			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision(enemy);

			// 敵弾の衝突時コールバックを呼び出す
			enemy->OnCollision(player_);
		}
	
	}

}
