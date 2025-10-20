#include "GameScene.h"

GameScene::~GameScene() {
	delete modelPlayer_;
	delete modelBlock_;
	delete modelEnemy_;
	delete modelSkydome_;
	delete modelParticle_;

	delete player_;
	delete mapChipField_;
	delete cameraController_;
	delete skydome_;
	if (deathParticles_)
		delete deathParticles_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	for (auto& worldTransformLine : worldTransformBlocks_) {
		for (WorldTransform* block : worldTransformLine) {
			delete block;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {

	phase_ = Phase::kPlay;

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

	// 敵生成
	Vector3 basePosition = {25.0f, 1.0f, 0.0f};
	Vector3 offset = {3.0f, 3.0f, 0.0f};

	for (int i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
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

	// deathParticles_ は nullptr で初期化するだけ
	deathParticles_ = nullptr;

	input_ = Input::GetInstance();

	worldTransform_.Initialize();

	GenerateBlocks();
}

void GameScene::Update() {

	// 先にフェーズ切替をチェック
	ChangePhase();

	switch (phase_) {

	case Phase::kPlay:

		if (input_->TriggerKey(DIK_Z)) {
			player_->Attack();
		}

		if (player_->IsAttacking()) {
			AABB attackBox = player_->GetAttackAABB();
			for (Enemy* enemy : enemies_) {
				if (AABB::IsCollision(attackBox, enemy->GetAABB())) {
					enemy->OnCollision(player_);
				}
			}
		}

		CheckAllCollision();

		player_->Update();
		for (Enemy* enemy : enemies_)
			enemy->Update();

		for (auto& line : worldTransformBlocks_) {
			for (auto* block : line) {
				if (block)
					block->UpdateMatrix();
			}
		}

		if (input_->TriggerKey(DIK_SPACE))
			isFinished_ = true;

		if (deathParticles_)
			deathParticles_->Update();

		cameraController_->Update();
		skydome_->Update();

		break;

	case Phase::kDeath:

		// 死亡中も敵・ブロック・パーティクルは更新
		for (Enemy* enemy : enemies_)
			enemy->Update();

		for (auto& line : worldTransformBlocks_) {
			for (auto* block : line) {
				if (block)
					block->UpdateMatrix();
			}
		}

		if (input_->TriggerKey(DIK_SPACE))
			isFinished_ = true;

		if (deathParticles_)
			deathParticles_->Update();

		cameraController_->Update();
		skydome_->Update();

		break;
	}
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Sprite::PostDraw();

	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	// ブロック描画
	for (auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			if (block)
				modelBlock_->Draw(*block, camera_);
		}
	}

	// プレイヤー描画
	if (phase_ == Phase::kPlay) {
		player_->Draw(camera_);
	}

	// 敵描画
	for (Enemy* enemy : enemies_)
		enemy->Draw();

	// 死亡パーティクル描画（nullptrチェック）
	if (deathParticles_)
		deathParticles_->Draw();

	skydome_->Draw();

	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Sprite::PostDraw();
}

void GameScene::GenerateBlocks() {

	worldTransformBlocks_.resize(MapChipField::kNumBlockVirtical);
	for (uint32_t i = 0; i < MapChipField::kNumBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(MapChipField::kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < MapChipField::kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < MapChipField::kNumBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j] = worldTransform;
			}
		}
	}
}

void GameScene::CheckAllCollision() {

	AABB playerAABB = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		if (AABB::IsCollision(playerAABB, enemy->GetAABB())) {
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
}

void GameScene::ChangePhase() {

	switch (phase_) {

	case Phase::kPlay:
		if (player_->IsDead()) {

			phase_ = Phase::kDeath;

			// 既存のdeathParticles_は削除
			if (deathParticles_) {
				delete deathParticles_;
				deathParticles_ = nullptr;
			}

			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_ = new DeathParticle();
			deathParticles_->Initialize(modelParticle_, &camera_, deathParticlesPosition);
		}
		break;

	case Phase::kDeath:
		break;
	}
}
