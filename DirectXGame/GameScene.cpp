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

	phase_ = Phase::kCountDown;
	countdownTimer_ = 1.5f;
	countdownNumber_ = 3;

	numberPos_ = {300, -500};
	startPos_ = {385, 280};

	modelPlayer_ = Model::CreateFromOBJ("Player");
	modelBlock_ = Model::CreateFromOBJ("block");
	modelEnemy_ = Model::CreateFromOBJ("enemy");
	modelSkydome_ = Model::CreateFromOBJ("skydome");
	modelParticle_ = Model::CreateFromOBJ("deathParticle");

	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCSV("Resources/map.csv");

	oneTextureHandle_ = TextureManager::Load("number/1.png");
	oneSprite_ = Sprite::Create(oneTextureHandle_, numberPos_);

	twoTextureHandle_ = TextureManager::Load("number/2.png");
	twoSprite_ = Sprite::Create(twoTextureHandle_, numberPos_);

	threeTextureHandle_ = TextureManager::Load("number/3.png");
	threeSprite_ = Sprite::Create(threeTextureHandle_, numberPos_);

	startTextureHandle_ = TextureManager::Load("number/start.png");
	startSprite_ = Sprite::Create(startTextureHandle_, startPos_);

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

	// フェーズ切替
	ChangePhase();

	switch (phase_) {

	case Phase::kCountDown:

		// 1秒だけ start を表示する場合
		countdownTimer_ -= 1.0f / 60.0f;
		if (countdownTimer_ <= 0.0f) {
			phase_ = Phase::kPlay; // すぐにゲーム開始
		}

		// プレイヤー操作無効
		player_->SetInputEnabled(false);
		player_->Update(); // 移動・攻撃は無効

		// 敵やブロックの更新
		for (Enemy* enemy : enemies_)
			enemy->Update();

		for (auto& line : worldTransformBlocks_) {
			for (auto* block : line) {
				if (block)
					block->UpdateMatrix();
			}
		}

		// カメラ・スカイドーム・死亡パーティクル更新
		cameraController_->Update();
		skydome_->Update();
		if (deathParticles_)
			deathParticles_->Update();

		break;

	case Phase::kPlay:
		// プレイヤー操作有効
		player_->SetInputEnabled(true);

		if (input_->TriggerKey(DIK_Z))
			player_->Attack();

		if (player_->IsAttacking()) {
			AABB attackBox = player_->GetAttackAABB();
			for (Enemy* enemy : enemies_) {
				if (AABB::IsCollision(attackBox, enemy->GetAABB()))
					enemy->OnCollision(player_);
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
		// 死亡中の更新
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

	// スプライト描画開始
	Sprite::PreDraw(dxCommon->GetCommandList());

	Sprite::PostDraw();

	// 3D描画開始
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	// ブロック描画
	for (auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			if (block)
				modelBlock_->Draw(*block, camera_);
		}
	}

	// プレイヤー描画（カウントダウン中も表示）
//	player_->Draw(camera_);
	if (phase_ != Phase::kDeath) {
		player_->Draw(camera_);
	}

	// 敵描画
	for (Enemy* enemy : enemies_)
		enemy->Draw();

	// 死亡パーティクル描画（nullptrチェック）
	if (deathParticles_)
		deathParticles_->Draw();

	// スカイドーム描画
	skydome_->Draw();

	Model::PostDraw();

	// 後処理スプライト描画（UIなど）
	Sprite::PreDraw(dxCommon->GetCommandList());

	// カウントダウンスプライト描画（拡大縮小なし）

	if (phase_ == Phase::kCountDown && startSprite_) {
		startSprite_->Draw();
	}

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
