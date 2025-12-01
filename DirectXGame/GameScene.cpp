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
	countdownTimer_ = 3.0f;
	countdownNumber_ = 3;

	numberPos_ = {300, -500};
	startPos_ = {135, 230};
	overPos_ = {140, 200};
	titlePos = {100, 400};

	modelPlayer_ = Model::CreateFromOBJ("Player");
	modelBlock_ = Model::CreateFromOBJ("block");
	modelFire_ = Model::CreateFromOBJ("fire"); // 追加：即死ブロックモデル読み込み
	modelGoal_ = Model::CreateFromOBJ("goal");
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

	overAlpha_ = 0.0f;
	startAlpha_ = 0.0f;
	clearAlpha_ = 0.0f;
	fadeAlpha_ = 0.0f;

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

    // --- プレイヤー更新 ---
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

        // 当たり判定チェック（即死ブロック含む）
        CheckAllCollision();

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
        // 落下中も描画・更新
        for (Enemy* enemy : enemies_)
            enemy->Update();

        for (auto& line : worldTransformBlocks_)
            for (auto* block : line)
                if (block)
                    block->UpdateMatrix();

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

        for (auto& line : worldTransformBlocks_)
            for (auto* block : line)
                if (block)
                    block->UpdateMatrix();

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

    // ブロック描画
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
            case MapChipType::kDamage:
                if (fireToggle_) {
                    modelFire_->Draw(*wt, camera_, fireTextureHandle1_);
                } else {
                    modelFire_->Draw(*wt, camera_, fireTextureHandle2_);
                }
                break;
            case MapChipType::kGoal:
                modelGoal_->Draw(*wt, camera_);
                break;
            }
        }
    }

    // プレイヤー描画：死亡中も落下中なら描画
    if (!player_->IsDead() || player_->IsFalling()) {
        player_->Draw(camera_);
    }

    for (Enemy* enemy : enemies_)
        enemy->Draw();

    if (deathParticles_)
        deathParticles_->Draw();

    skydome_->Draw();
    Model::PostDraw();

    Sprite::PreDraw(dxCommon->GetCommandList());

    if (phase_ == Phase::kCountDown && startSprite_) {
        float alpha = startAlpha_;
        startSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
        startSprite_->Draw();
    }

    if (fadeSprite_) {
        fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
        fadeSprite_->Draw();
    }

    if (phase_ == Phase::kDeath) {
        float alpha = overAlpha_;
        overSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
        overSprite_->Draw();
    }

    if (phase_ == Phase::kGoal && clearSprite_) {
        clearSprite_->SetColor({1.0f, 1.0f, 1.0f, clearAlpha_});
        clearSprite_->Draw();
    }

    Sprite::PostDraw();
}


//void GameScene::Update() {
//
//	// フェーズ切替
//	ChangePhase();
//
//	fireSwitchTimer_ += 1.0f / 60.0f; // フレームごとに加算
//	if (fireSwitchTimer_ >= fireSwitchInterval_) {
//		fireSwitchTimer_ = 0.0f;
//		fireToggle_ = !fireToggle_;
//	}
//
//		player_->StartDeathFall();
//
//	switch (phase_) {
//
//	case Phase::kCountDown:
//		countdownTimer_ -= 1.0f / 60.0f;
//		if (countdownTimer_ <= 0.0f) {
//			phase_ = Phase::kPlay;
//		}
//
//		if (startAlpha_ < 1.0f) {
//			startAlpha_ += 1.0f / 120.0f; // 2秒でフル表示
//			if (startAlpha_ > 1.0f)
//				startAlpha_ = 1.0f;
//		}
//
//		player_->SetInputEnabled(false);
//		player_->Update();
//
//		// if (player_->IsDead() && !player_->IsFalling()) {
//		//	player_->StartDeathFall(); // 落下演出開始
//		//}
//
//		for (Enemy* enemy : enemies_)
//			enemy->Update();
//
//		for (auto& line : worldTransformBlocks_)
//			for (auto* block : line)
//				if (block)
//					block->UpdateMatrix();
//
//		cameraController_->Update();
//		skydome_->Update();
//		if (deathParticles_)
//			deathParticles_->Update();
//
//		break;
//
//	case Phase::kPlay:
//		player_->SetInputEnabled(true);
//
//		// 当たり判定チェック（即死ブロック含む）
//		CheckAllCollision();
//
//		player_->Update();
//		for (Enemy* enemy : enemies_)
//			enemy->Update();
//
//		for (auto& line : worldTransformBlocks_)
//			for (auto* block : line)
//				if (block)
//					block->UpdateMatrix();
//
//		if (deathParticles_)
//			deathParticles_->Update();
//
//		cameraController_->Update();
//		skydome_->Update();
//
//		break;
//
//	case Phase::kDeath:
//		for (Enemy* enemy : enemies_)
//			enemy->Update();
//
//		for (auto& line : worldTransformBlocks_)
//			for (auto* block : line)
//				if (block)
//					block->UpdateMatrix();
//
//		 if (fadeAlpha_ < 1.0f) {
//			fadeAlpha_ += fadeSpeed_;
//			if (fadeAlpha_ > 1.0f)
//				fadeAlpha_ = 1.0f;
//		}
//
//		if (overAlpha_ < 1.0f) {
//			overAlpha_ += 1.0f / 180.0f;
//			if (overAlpha_ > 1.0f) {
//				overAlpha_ = 1.0f;
//			}
//		}
//
//		if (input_->TriggerKey(DIK_SPACE))
//			isFinished_ = true;
//
//		if (deathParticles_)
//			deathParticles_->Update();
//
//		cameraController_->Update();
//		skydome_->Update();
//		break;
//
//		case Phase::kGoal:
//		// プレイヤー操作を無効化
//		player_->SetInputEnabled(false);
//
//		// ゴールパーティクル生成（最初のフレームだけ）
//		if (!deathParticles_) {
//			deathParticles_ = new DeathParticle();
//			deathParticles_->Initialize(modelParticle_, &camera_, player_->GetWorldPosition());
//		}
//
//		// パーティクル更新
//		if (deathParticles_)
//			deathParticles_->Update();
//
//		// 徐々に暗転
//		if (fadeAlpha_ < 1.0f) {
//			fadeAlpha_ += fadeSpeed_; // fadeSpeed_ は 1/180〜1/120 程度が目安
//			if (fadeAlpha_ > 1.0f)
//				fadeAlpha_ = 1.0f;
//		}
//
//		// 暗転完了後にクリア画面を徐々に表示
//		if (fadeAlpha_ >= 1.0f) {
//			if (clearAlpha_ < 1.0f) {
//				clearAlpha_ += 1.0f / 180.0f; // 3秒でフル表示
//				if (clearAlpha_ > 1.0f)
//					clearAlpha_ = 1.0f;
//			}
//		}
//
//		// プレイヤー・敵・ブロックの更新
//		player_->Update();
//		for (Enemy* enemy : enemies_)
//			enemy->Update();
//		for (auto& line : worldTransformBlocks_)
//			for (auto* block : line)
//				if (block)
//					block->UpdateMatrix();
//
//		cameraController_->Update();
//		skydome_->Update();
//
//		// スペースキーでシーン終了
//		if (input_->TriggerKey(DIK_SPACE))
//			isFinished_ = true;
//
//		break;
//
//	}
//}
//
//void GameScene::Draw() {
//	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
//
//	Sprite::PreDraw(dxCommon->GetCommandList());
//	Sprite::PostDraw();
//
//	dxCommon->ClearDepthBuffer();
//	Model::PreDraw();
//
//	// ブロック描画（通常と即死を分けて描画）
//	for (uint32_t y = 0; y < mapChipField_->GetNumBlockVirtical(); ++y) {
//		for (uint32_t x = 0; x < mapChipField_->GetNumBlockHorizontal(); ++x) {
//
//			MapChipType type = mapChipField_->GetMapChipTypeByIndex(x, y);
//			if (type == MapChipType::kBlank)
//				continue;
//
//			WorldTransform* wt = worldTransformBlocks_[y][x];
//			if (!wt)
//				continue;
//
//			switch (type) {
//			case MapChipType::kBlock:
//				modelBlock_->Draw(*wt, camera_);
//				break;
//			case MapChipType::kDamage: {
//				if (fireToggle_) {
//					modelFire_->Draw(*wt, camera_, fireTextureHandle1_);
//				} else {
//					modelFire_->Draw(*wt, camera_, fireTextureHandle2_);
//				}
//				break;
//			}
//
//			case MapChipType::kGoal:
//				modelGoal_->Draw(*wt, camera_);
//				break;
//			}
//		}
//	}
//
//	/*if (phase_ != Phase::kDeath)
//		player_->Draw(camera_);*/
//
//	/*if (!player_->IsDead() || player_->IsFalling()) {
//		player_->Draw(camera_);
//	}*/
//
//	player_->Draw(camera_);
//
//	for (Enemy* enemy : enemies_)
//		enemy->Draw();
//
//	if (deathParticles_)
//		deathParticles_->Draw();
//
//	skydome_->Draw();
//
//	Model::PostDraw();
//
//	Sprite::PreDraw(dxCommon->GetCommandList());
//
//	if (phase_ == Phase::kCountDown && startSprite_) {
//		float alpha = startAlpha_;                         // 0.0〜1.0
//		startSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha}); // 白に透明度を乗算
//		startSprite_->Draw();
//	}
//
//	 if (fadeSprite_) {
//		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
//		fadeSprite_->Draw(); // サイズ指定なし
//	}
//
//	if (phase_ == Phase::kDeath) {
//		float alpha = overAlpha_;                         // 0.0〜1.0の範囲
//		overSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha}); // 白を乗算
//		overSprite_->Draw();
//	}
//
//	if (phase_ == Phase::kGoal && clearSprite_) {
//		clearSprite_->SetColor({1.0f, 1.0f, 1.0f, clearAlpha_});
//		clearSprite_->Draw();
//	}
//
//	Sprite::PostDraw();
//}

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

		if (player_->IsGoal()) {
			phase_ = Phase::kGoal; // 新しくゴールフェーズを作る
		}

		break;

	case Phase::kDeath:
		break;

	case Phase::kGoal:
		// ゴールフェーズでの処理（クリア画面表示など）

		break;
	}
}
