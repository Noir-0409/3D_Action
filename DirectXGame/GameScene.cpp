#include "GameScene.h"
#include "BlockTypes.h"

GameScene::~GameScene() {

	// モデルの解放
	delete modelPlayer_;
	delete modelBlock_;
	delete modelFire_;
	delete modelEnemy_;
	delete modelSkydome_;
	delete modelParticle_;

	// ゲームオブジェクトの解放
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

	// 新しい管理配列 blocks_ を安全に解放
	for (auto& blockLine : blocks_) {
		for (BaseBlock* block : blockLine) {
			if (block) {
				delete block;
			}
		}
	}
	blocks_.clear();
}

void GameScene::Initialize() {

	// フェーズ初期化
	phase_ = Phase::kCountDown;
	countdownTimer_ = 3.0f;
	countdownNumber_ = 3;

	// UI表示位置
	numberPos_ = {300, -500};
	startPos_ = {135, 230};
	overPos_ = {140, 200};
	titlePos = {100, 400};
	guidePos_ = {0, 400};

	// モデル読み込み
	modelBlock_ = Model::CreateFromOBJ("block");
	modelFire_ = Model::CreateFromOBJ("fire");
	modelGoal_ = Model::CreateFromOBJ("goal");
	modelEnemy_ = Model::CreateFromOBJ("enemy");
	modelSkydome_ = Model::CreateFromOBJ("skydome");
	modelParticle_ = Model::CreateFromOBJ("deathParticle");
	modelIce_ = Model::CreateFromOBJ("ice");
	modelRed_ = Model::CreateFromOBJ("red");
	modelBlue_ = Model::CreateFromOBJ("blue");
	modelRed2_ = Model::CreateFromOBJ("red2");
	modelBlue2_ = Model::CreateFromOBJ("blue2");
	modelPlayer_ = Model::CreateFromOBJ("Player2");

	// マップチップ読み込み
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCSV("Resources/map.csv");

	// スプライト読み込み
	oneTextureHandle_ = TextureManager::Load("number/1.png");
	oneSprite_ = Sprite::Create(oneTextureHandle_, numberPos_);

	twoTextureHandle_ = TextureManager::Load("number/2.png");
	twoSprite_ = Sprite::Create(twoTextureHandle_, numberPos_);

	threeTextureHandle_ = TextureManager::Load("number/3.png");
	threeSprite_ = Sprite::Create(threeTextureHandle_, numberPos_);

	startTextureHandle_ = TextureManager::Load("gamestart.png");
	startSprite_ = Sprite::Create(startTextureHandle_, startPos_);

	clearTextureHandle_ = TextureManager::Load("clear.png");
	clearSprite_ = Sprite::Create(clearTextureHandle_, startPos_);

	fireTextureHandle1_ = TextureManager::Load("fire/fire1.png");
	fireTextureHandle2_ = TextureManager::Load("fire/fire2.png");

	overTextureHandle_ = TextureManager::Load("gameover.png");
	overSprite_ = Sprite::Create(overTextureHandle_, overPos_);

	fadeTextureHandle_ = TextureManager::Load("black.png");
	fadeSprite_ = Sprite::Create(fadeTextureHandle_, {0, 0});

	overTextureHandle_ = TextureManager::Load("overTitle.png");
	overTitleSprite_ = Sprite::Create(overTitleTextureHandle_, titlePos);

	guideTextureHandle_ = TextureManager::Load("keyGuide.png");
	guideSprite_ = Sprite::Create(guideTextureHandle_, guidePos_);

	camera_.Initialize();

	// プレイヤー生成
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 24);
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

	// カメラコントローラ設定
	cameraController_ = new CameraController();
	cameraController_->SetCamera(&camera_);
	cameraController_->SetTarget(player_);
	cameraController_->Initialize();
	cameraController_->Reset();

	// スカイドーム生成
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	// その他初期化
	deathParticles_ = nullptr;

	input_ = Input::GetInstance();

	overAlpha_ = 0.0f;
	startAlpha_ = 0.0f;
	clearAlpha_ = 0.0f;
	fadeAlpha_ = 0.0f;

	worldTransform_.Initialize();

	GenerateBlocks();
}

void GameScene::Update(float deltaTime) {

	deltaTime;

	mapChipField_->Update();

	// フェーズ切替
	ChangePhase();

	// 即死ブロックの炎アニメ切替
	fireSwitchTimer_ += 1.0f / 60.0f;
	if (fireSwitchTimer_ >= fireSwitchInterval_) {
		fireSwitchTimer_ = 0.0f;
		fireToggle_ = !fireToggle_;
	}

	player_->Update();

	// 死亡判定で落下開始
	if (player_->IsDead() && !player_->IsFalling()) {
		player_->StartDeathFall(); // 落下演出開始
	}

	switch (phase_) {
	case Phase::kCountDown:
		countdownTimer_ -= 1.0f / 60.0f;
		if (countdownTimer_ <= 0.0f) {
			phase_ = Phase::kPlay;
		}

		if (startAlpha_ < 1.0f) {
			startAlpha_ += 1.0f / 120.0f; // 2秒でフル表示
			if (startAlpha_ > 1.0f)
				startAlpha_ = 1.0f;
		}

		player_->SetInputEnabled(false);

		for (Enemy* enemy : enemies_)
			enemy->Update();

		// ★【修正箇所】blocks_ 経由で各ブロックを行列更新
		for (auto& line : blocks_) {
			for (auto* block : line) {
				if (block) {
					WorldTransform* wt = block->GetWorldTransform();
					if (wt)
						wt->UpdateMatrix();
				}
			}
		}

		cameraController_->Update();
		skydome_->Update();
		if (deathParticles_)
			deathParticles_->Update();

		break;

	case Phase::kPlay:
		player_->SetInputEnabled(true);

		// 当たり判定チェック（即死ブロック含む）
		CheckAllCollision();

		for (Enemy* enemy : enemies_)
			enemy->Update();

		// ★【修正箇所】blocks_ 経由で各ブロックを行列更新
		for (auto& line : blocks_) {
			for (auto* block : line) {
				if (block) {
					WorldTransform* wt = block->GetWorldTransform();
					if (wt)
						wt->UpdateMatrix();
				}
			}
		}

		if (deathParticles_)
			deathParticles_->Update();

		cameraController_->Update();
		skydome_->Update();

		break;

	case Phase::kDeath:
		// 落下中も描画・更新
		for (Enemy* enemy : enemies_)
			enemy->Update();

		// ★【修正箇所】blocks_ 経由で各ブロックを行列更新
		for (auto& line : blocks_) {
			for (auto* block : line) {
				if (block) {
					WorldTransform* wt = block->GetWorldTransform();
					if (wt)
						wt->UpdateMatrix();
				}
			}
		}

		if (fadeAlpha_ < 1.0f) {
			fadeAlpha_ += fadeSpeed_;
			if (fadeAlpha_ > 1.0f)
				fadeAlpha_ = 1.0f;
		}

		if (overAlpha_ < 1.0f) {
			overAlpha_ += 1.0f / 180.0f;
			if (overAlpha_ > 1.0f)
				overAlpha_ = 1.0f;
		}

		if (input_->TriggerKey(DIK_SPACE))
			isFinished_ = true;

		if (deathParticles_)
			deathParticles_->Update();

		cameraController_->Update();
		skydome_->Update();
		break;

	case Phase::kGoal:
		player_->SetInputEnabled(false);

		if (!deathParticles_) {
			deathParticles_ = new DeathParticle();
			deathParticles_->Initialize(modelParticle_, &camera_, player_->GetWorldPosition());
		}

		if (deathParticles_)
			deathParticles_->Update();

		if (fadeAlpha_ < 1.0f) {
			fadeAlpha_ += fadeSpeed_;
			if (fadeAlpha_ > 1.0f)
				fadeAlpha_ = 1.0f;
		}

		if (fadeAlpha_ >= 1.0f) {
			if (clearAlpha_ < 1.0f) {
				clearAlpha_ += 1.0f / 180.0f;
				if (clearAlpha_ > 1.0f)
					clearAlpha_ = 1.0f;
			}
		}

		for (Enemy* enemy : enemies_)
			enemy->Update();

		// ★【修正箇所】blocks_ 経由で各ブロックを行列更新
		for (auto& line : blocks_) {
			for (auto* block : line) {
				if (block) {
					WorldTransform* wt = block->GetWorldTransform();
					if (wt)
						wt->UpdateMatrix();
				}
			}
		}

		cameraController_->Update();
		skydome_->Update();

		if (input_->TriggerKey(DIK_SPACE))
			isFinished_ = true;

		break;
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Sprite::PostDraw();

	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	// ★【修正箇所】すべてのブロックをポリモーフィズムで一括描画！
	// SwitchBlock の内部で「通常状態」と「消滅中の半透明状態」の描き分けも自動で行われます。
	for (uint32_t y = 0; y < MapChipField::kNumBlockVirtical; ++y) {
		for (uint32_t x = 0; x < MapChipField::kNumBlockHorizontal; ++x) {
			if (blocks_[y][x]) {
				blocks_[y][x]->Draw(camera_);
			}
		}
	}

	// プレイヤー
	if (!player_->IsDead() || player_->IsFalling()) {
		player_->Draw();
	}

	// その他
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	skydome_->Draw();

	Model::PostDraw();

	// スプライト
	Sprite::PreDraw(dxCommon->GetCommandList());

	if (phase_ == Phase::kCountDown && startSprite_) {
		startSprite_->SetColor({1.0f, 1.0f, 1.0f, startAlpha_});
		startSprite_->Draw();
	}

	if (fadeSprite_) {
		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
		fadeSprite_->Draw();
	}

	if (phase_ == Phase::kPlay) {
		guideSprite_->Draw();
	}

	if (phase_ == Phase::kDeath) {
		overSprite_->SetColor({1.0f, 1.0f, 1.0f, overAlpha_});
		overSprite_->Draw();
	}

	if (phase_ == Phase::kGoal && clearSprite_) {
		clearSprite_->SetColor({1.0f, 1.0f, 1.0f, clearAlpha_});
		clearSprite_->Draw();
	}

	Sprite::PostDraw();
}

void GameScene::GenerateBlocks() {

	// 配列のサイズをマップチップの大きさに合わせて確保
	blocks_.resize(MapChipField::kNumBlockVirtical);
	for (uint32_t i = 0; i < MapChipField::kNumBlockVirtical; ++i) {
		blocks_[i].resize(MapChipField::kNumBlockHorizontal, nullptr);
	}

	// マップを1マスずつ走査してブロックを生成
	for (uint32_t y = 0; y < MapChipField::kNumBlockVirtical; ++y) {
		for (uint32_t x = 0; x < MapChipField::kNumBlockHorizontal; ++x) {

			MapChipType type = mapChipField_->GetRawMapChipTypeByIndex(x, y);

			// 空白のマスは何もしない
			if (type == MapChipType::kBlank)
				continue;

			// ブロックの位置情報（WorldTransform）を新しく作成
			WorldTransform* wt = new WorldTransform();
			wt->Initialize();
			wt->translation_ = mapChipField_->GetMapChipPositionByIndex(x, y);

			// ★【ここがポリモーフィズム】ブロックの種類（Type）に合わせて、生成する子クラスを切り替える
			if (type == MapChipType::kBlock) {
				// 通常ブロック
				blocks_[y][x] = new NormalBlock(modelBlock_, wt);
			} else if (type == MapChipType::kDamage) {
				// 炎ブロック（アニメーション用のトグルフラグと、2つのテクスチャを渡す）
				blocks_[y][x] = new FireBlock(modelFire_, wt, fireToggle_, fireTextureHandle1_, fireTextureHandle2_);
			} else if (type == MapChipType::kRed || type == MapChipType::kBlue) {
				// スイッチブロック（現在の状態を取得する関数と、自分が赤か青かの情報を渡す）
				auto getMapState = [this]() { return mapChipField_->GetMapChipType(); };

				Model* normalModel = (type == MapChipType::kRed) ? modelRed_ : modelBlue_;
				Model* vanishedModel = (type == MapChipType::kRed) ? modelRed2_ : modelBlue2_;

				blocks_[y][x] = new SwitchBlock(normalModel, vanishedModel, wt, getMapState, type);
			} else {
				// 氷ブロックやゴールブロックなど、その他の通常描画ブロック
				Model* targetModel = modelBlock_;
				if (type == MapChipType::kIce)
					targetModel = modelIce_;
				if (type == MapChipType::kGoal)
					targetModel = modelGoal_;

				blocks_[y][x] = new NormalBlock(targetModel, wt);
			}
		}
	}
}

void GameScene::CheckAllCollision() {

	// プレイヤーのAABB取得
	AABB playerAABB = player_->GetAABB();

	// 敵との当たり判定
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

		// プレイヤー死亡 → デスフェーズ
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

		// ゴール到達 → ゴールフェーズ
		if (player_->IsGoal()) {
			phase_ = Phase::kGoal;
		}

		break;

	case Phase::kDeath:

		break;

	case Phase::kGoal:

		break;
	}
}