#include "GameScene.h"

GameScene::~GameScene() {
	delete modelPlayer_;
	delete modelBlock_;
	delete modelFire_; // 追加：即死ブロックモデル
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
	modelFire_ = Model::CreateFromOBJ("fire"); // 追加：即死ブロックモデル読み込み
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

	fireTextureHandle1_ = TextureManager::Load("fire/fire1.png");
	fireTextureHandle2_ = TextureManager::Load("fire/fire2.png");

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

	deathParticles_ = nullptr;

	input_ = Input::GetInstance();

	worldTransform_.Initialize();

	GenerateBlocks();
}

void GameScene::Update() {

	// フェーズ切替
	ChangePhase();

	fireSwitchTimer_ += 1.0f / 60.0f; // フレームごとに加算
	if (fireSwitchTimer_ >= fireSwitchInterval_) {
		fireSwitchTimer_ = 0.0f;
		fireToggle_ = !fireToggle_;
	}

	

	switch (phase_) {

	case Phase::kCountDown:
		countdownTimer_ -= 1.0f / 60.0f;
		if (countdownTimer_ <= 0.0f) {
			phase_ = Phase::kPlay;
		}

		player_->SetInputEnabled(false);
		player_->Update();

		for (Enemy* enemy : enemies_)
			enemy->Update();

		for (auto& line : worldTransformBlocks_)
			for (auto* block : line)
				if (block)
					block->UpdateMatrix();

		cameraController_->Update();
		skydome_->Update();
		if (deathParticles_)
			deathParticles_->Update();

		break;

	case Phase::kPlay:
		player_->SetInputEnabled(true);

		if (input_->TriggerKey(DIK_Z))
			player_->Attack();

		if (player_->IsAttacking()) {
			AABB attackBox = player_->GetAttackAABB();
			for (Enemy* enemy : enemies_)
				if (AABB::IsCollision(attackBox, enemy->GetAABB()))
					enemy->OnCollision(player_);
		}

		// 当たり判定チェック（即死ブロック含む）
		CheckAllCollision();

		player_->Update();
		for (Enemy* enemy : enemies_)
			enemy->Update();

		for (auto& line : worldTransformBlocks_)
			for (auto* block : line)
				if (block)
					block->UpdateMatrix();

		if (deathParticles_)
			deathParticles_->Update();

		cameraController_->Update();
		skydome_->Update();

		break;

	case Phase::kDeath:
		for (Enemy* enemy : enemies_)
			enemy->Update();

		for (auto& line : worldTransformBlocks_)
			for (auto* block : line)
				if (block)
					block->UpdateMatrix();

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

	// ブロック描画（通常と即死を分けて描画）
	for (uint32_t y = 0; y < mapChipField_->GetNumBlockVirtical(); ++y) {
		for (uint32_t x = 0; x < mapChipField_->GetNumBlockHorizontal(); ++x) {

			MapChipType type = mapChipField_->GetMapChipTypeByIndex(x, y);
			if (type == MapChipType::kBlank)
				continue;

			WorldTransform* wt = worldTransformBlocks_[y][x];
			if (!wt)
				continue;

			switch (type) {
			case MapChipType::kBlock:
				modelBlock_->Draw(*wt, camera_);
				break;
			case MapChipType::kDamage: {
				if (fireToggle_) {
					modelFire_->Draw(*wt, camera_, fireTextureHandle1_);
				} else {
					modelFire_->Draw(*wt, camera_, fireTextureHandle2_);
				}
				break;
			}
			}
		}
	}

	if (phase_ != Phase::kDeath)
		player_->Draw(camera_);

	for (Enemy* enemy : enemies_)
		enemy->Draw();

	if (deathParticles_)
		deathParticles_->Draw();

	skydome_->Draw();

	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());
	if (phase_ == Phase::kCountDown && startSprite_)
		startSprite_->Draw();
	Sprite::PostDraw();
}

void GameScene::GenerateBlocks() {
	worldTransformBlocks_.resize(MapChipField::kNumBlockVirtical);
	for (uint32_t i = 0; i < MapChipField::kNumBlockVirtical; ++i)
		worldTransformBlocks_[i].resize(MapChipField::kNumBlockHorizontal);

	for (uint32_t y = 0; y < MapChipField::kNumBlockVirtical; ++y) {
		for (uint32_t x = 0; x < MapChipField::kNumBlockHorizontal; ++x) {
			MapChipType type = mapChipField_->GetMapChipTypeByIndex(x, y);
			if (type == MapChipType::kBlank)
				continue;

			WorldTransform* wt = new WorldTransform();
			wt->Initialize();
			wt->translation_ = mapChipField_->GetMapChipPositionByIndex(x, y);
			worldTransformBlocks_[y][x] = wt;
		}
	}
}

void GameScene::CheckAllCollision() {

	AABB playerAABB = player_->GetAABB();

	// --- 敵との当たり判定 ---
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
