#include "GameScene.h"
#include "BlockTypes.h"
#include "CollisionObserver.h"
#include "ConcreteGamePhases.h"
#include "GameObjectFactory.h"

// ⭕ 指摘事項1：デストラクタ内の手動 delete をすべて排除！
GameScene::~GameScene() {

	// unique_ptrが自動破棄してくれるため、player_、blocks_、mapChipField_、sprites_ などのdeleteは一切不要です！
	gameObjects_.clear();
	enemies_.clear();
	blocks_.clear();
}

void GameScene::Initialize() {
	countdownTimer_ = 3.0f;
	countdownNumber_ = 3;

	numberPos_ = {300, -500};
	startPos_ = {135, 230};
	overPos_ = {140, 200};
	titlePos = {100, 400};
	guidePos_ = {0, 400};

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

	// ⭕ new を排除してスマートポインタで生成
	mapChipField_ = std::make_unique<MapChipField>();
	mapChipField_->LoadMapChipCSV("Resources/map.csv");

	// 各種スプライト生成も std::unique_ptr (実体は Sprite::Create が生ポインタを返すため unique_ptr に包む)
	oneTextureHandle_ = TextureManager::Load("number/1.png");
	oneSprite_.reset(Sprite::Create(oneTextureHandle_, numberPos_));

	twoTextureHandle_ = TextureManager::Load("number/2.png");
	twoSprite_.reset(Sprite::Create(twoTextureHandle_, numberPos_));

	threeTextureHandle_ = TextureManager::Load("number/3.png");
	threeSprite_.reset(Sprite::Create(threeTextureHandle_, numberPos_));

	startTextureHandle_ = TextureManager::Load("gamestart.png");
	startSprite_.reset(Sprite::Create(startTextureHandle_, startPos_));

	clearTextureHandle_ = TextureManager::Load("clear.png");
	clearSprite_.reset(Sprite::Create(clearTextureHandle_, startPos_));

	fireTextureHandle1_ = TextureManager::Load("fire/fire1.png");
	fireTextureHandle2_ = TextureManager::Load("fire/fire2.png");

	overTextureHandle_ = TextureManager::Load("gameover.png");
	overSprite_.reset(Sprite::Create(overTextureHandle_, overPos_));

	fadeTextureHandle_ = TextureManager::Load("black.png");
	fadeSprite_.reset(Sprite::Create(fadeTextureHandle_, {0, 0}));

	overTextureHandle_ = TextureManager::Load("overTitle.png");
	overTitleSprite_.reset(Sprite::Create(overTitleTextureHandle_, titlePos));

	guideTextureHandle_ = TextureManager::Load("keyGuide.png");
	guideSprite_.reset(Sprite::Create(guideTextureHandle_, guidePos_));

	camera_.Initialize();
	gameObjects_.clear();

	// 1. スカイドームの生成
	skydome_ = GameObjectFactory::CreateSkydome(modelSkydome_, &camera_);
	gameObjects_.push_back(skydome_.get()); // 寿命はskydome_が持つので、リストへは.get()で生ポインタを登録

	// 2. プレイヤーの生成
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 24);
	player_ = GameObjectFactory::CreatePlayer(modelPlayer_, &camera_, playerPosition, mapChipField_.get());
	gameObjects_.push_back(player_.get());

	// 3. 敵の生成
	Vector3 basePosition = {25.0f, 1.0f, 0.0f};
	Vector3 offset = {3.0f, 3.0f, 0.0f};

	enemyOwnerList_.clear();

	for (int i = 0; i < 3; ++i) {
		Vector3 enemyPosition = basePosition + offset * static_cast<float>(i);

		// 工場から unique_ptr を受け取る
		std::unique_ptr<Enemy> newEnemy = GameObjectFactory::CreateEnemy(modelEnemy_, &camera_, enemyPosition);

		enemies_.push_back(newEnemy.get());     // 判定用非所有ポインタ
		gameObjects_.push_back(newEnemy.get()); // 描画・更新用非所有ポインタ

		enemyOwnerList_.push_back(std::move(newEnemy));

	}

	collisionObservers_.push_back(std::make_unique<PlayerEnemyCollisionHandler>());

	// ⭕ new を排除
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->SetCamera(&camera_);
	cameraController_->SetTarget(player_.get());
	cameraController_->Initialize();
	cameraController_->Reset();

	deathParticles_ = nullptr;
	input_ = Input::GetInstance();
	overAlpha_ = 0.0f;
	startAlpha_ = 0.0f;
	clearAlpha_ = 0.0f;
	fadeAlpha_ = 0.0f;

	worldTransform_.Initialize();
	GenerateBlocks();

	phaseState_ = std::make_unique<CountDownPhase>();
}

void GameScene::Update(float deltaTime) {
	(void)deltaTime;
	mapChipField_->Update();

	fireSwitchTimer_ += 1.0f / 60.0f;
	if (fireSwitchTimer_ >= fireSwitchInterval_) {
		fireSwitchTimer_ = 0.0f;
		fireToggle_ = !fireToggle_;
	}

	for (GameObject* obj : gameObjects_) {
		if (obj) {
			obj->Update();
		}
	}

	NotifyCollisions();

	if (player_->IsDead() && !player_->IsFalling()) {
		player_->StartDeathFall();
	}

	if (phaseState_) {
		phaseState_->Update(this);
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Sprite::PostDraw();

	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	for (uint32_t y = 0; y < MapChipField::kNumBlockVirtical; ++y) {
		for (uint32_t x = 0; x < MapChipField::kNumBlockHorizontal; ++x) {
			if (blocks_[y][x]) {
				blocks_[y][x]->Draw(camera_);
			}
		}
	}

	for (GameObject* obj : gameObjects_) {
		if (obj) {
			if (obj == player_.get()) {
				if (!player_->IsDead() || player_->IsFalling()) {
					obj->Draw();
				}
			} else {
				obj->Draw();
			}
		}
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());

	if (phaseState_) {
		phaseState_->Draw(this);
	}

	if (fadeSprite_) {
		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
		fadeSprite_->Draw();
	}

	Sprite::PostDraw();
}

void GameScene::ChangePhase(std::unique_ptr<GamePhaseState> newPhase) { phaseState_ = std::move(newPhase); }

// ⭕ 指摘事項4：GenerateBlocks 関数内の new をすべて排除！
void GameScene::GenerateBlocks() {
	// 1. unique_ptr の vector なので、clearするだけで古いブロックは自動安全に解放（delete）されます！
	blocks_.clear();

	blocks_.resize(MapChipField::kNumBlockVirtical);
	for (uint32_t y = 0; y < MapChipField::kNumBlockVirtical; ++y) {
		blocks_[y].resize(MapChipField::kNumBlockHorizontal);
	}

	for (uint32_t y = 0; y < MapChipField::kNumBlockVirtical; ++y) {
		for (uint32_t x = 0; x < MapChipField::kNumBlockHorizontal; ++x) {

			MapChipType type = mapChipField_->GetMapChipTypeByIndex(x, y);

			if (type == MapChipType::kBlank) {
				continue;
			}

			Vector3 blockPosition = mapChipField_->GetMapChipPositionByIndex(x, y);

			// ❌ ポインタのnewを排除し、実体としてWorldTransformを持たせるか、スマートポインタ化
			// BaseBlock側が「WorldTransform*」を要求しているため、BaseBlockの設計に合わせる必要があります。
			// 指摘事項3「BaseBlock.h のデストラクタにある delete」をクリアするために、
			// BaseBlockがWorldTransformを「std::unique_ptr<WorldTransform>」で管理するように変更します。
			auto wt = std::make_unique<WorldTransform>();
			wt->Initialize();
			wt->translation_ = blockPosition;

			if (type == MapChipType::kRed || type == MapChipType::kBlue) {
				auto getMapState = [this]() { return mapChipField_->GetMapChipType(); };

				Model* normalModel = (type == MapChipType::kRed) ? modelRed_ : modelBlue_;
				Model* vanishedModel = (type == MapChipType::kRed) ? modelRed2_ : modelBlue2_;

				// ⭕ new SwitchBlock ではなく std::make_unique<SwitchBlock> を使用
				// wt は所有権を移譲 (std::move) します
				blocks_[y][x] = std::make_unique<SwitchBlock>(normalModel, vanishedModel, std::move(wt), getMapState, type);
			} else {
				Model* targetModel = modelBlock_;

				if (type == MapChipType::kIce) {
					targetModel = modelIce_;
				}
				if (type == MapChipType::kGoal) {
					targetModel = modelGoal_;
				}
				if (type == MapChipType::kDamage) {
					targetModel = modelFire_;
				}

				// ⭕ new NormalBlock ではなく std::make_unique<NormalBlock> を使用
				blocks_[y][x] = std::make_unique<NormalBlock>(targetModel, std::move(wt));
			}
		}
	}
}

void GameScene::NotifyCollisions() {
	AABB playerAABB = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		if (AABB::IsCollision(playerAABB, enemy->GetAABB())) {
			for (const auto& observer : collisionObservers_) {
				observer->OnPlayerEnemyCollision(player_.get(), enemy);
			}
		}
	}
}