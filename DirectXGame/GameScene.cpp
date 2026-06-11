#include "GameScene.h"
#include "BlockTypes.h"
#include "ConcreteGamePhases.h" // ★ 追加

GameScene::~GameScene() {
	delete modelPlayer_;
	delete modelBlock_;
	delete modelFire_;
	delete modelEnemy_;
	delete modelSkydome_;
	delete modelParticle_;

	for (GameObject* obj : gameObjects_) {
		if (obj) {
			delete obj;
		}
	}
	gameObjects_.clear();

	delete mapChipField_;
	delete cameraController_;
	if (deathParticles_)
		delete deathParticles_;

	enemies_.clear();

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

	blocks_.resize(MapChipField::kNumBlockVirtical);
	for (uint32_t y = 0; y < MapChipField::kNumBlockVirtical; ++y) {
		blocks_[y].resize(MapChipField::kNumBlockHorizontal, nullptr); // 全て nullptr で初期化
	}

	// ★ 初期フェーズ状態を「カウントダウン状態」に設定
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

	guideTextureHandle_ = TextureManager::Load("keyGuide.png");
	guideSprite_ = Sprite::Create(guideTextureHandle_, guidePos_);

	camera_.Initialize();
	gameObjects_.clear();

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);
	gameObjects_.push_back(skydome_);

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 24);
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);
	gameObjects_.push_back(player_);

	Vector3 basePosition = {25.0f, 1.0f, 0.0f};
	Vector3 offset = {3.0f, 3.0f, 0.0f};

	for (int i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = basePosition + offset * static_cast<float>(i);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
		gameObjects_.push_back(newEnemy);
	}

	cameraController_ = new CameraController();
	cameraController_->SetCamera(&camera_);
	cameraController_->SetTarget(player_);
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

	// 即死ブロックの炎アニメ切替
	fireSwitchTimer_ += 1.0f / 60.0f;
	if (fireSwitchTimer_ >= fireSwitchInterval_) {
		fireSwitchTimer_ = 0.0f;
		fireToggle_ = !fireToggle_;
	}

	// すべてのゲームオブジェクトを一括更新
	for (GameObject* obj : gameObjects_) {
		if (obj) {
			obj->Update();
		}
	}

	// 死亡判定で落下開始
	if (player_->IsDead() && !player_->IsFalling()) {
		player_->StartDeathFall();
	}

	// ★ 現在のフェーズの更新処理を呼び出す（ポリモーフィズム）
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

	// 1. マップブロックの描画
	for (uint32_t y = 0; y < MapChipField::kNumBlockVirtical; ++y) {
		for (uint32_t x = 0; x < MapChipField::kNumBlockHorizontal; ++x) {
			if (blocks_[y][x]) {
				blocks_[y][x]->Draw(camera_);
			}
		}
	}

	// 2. すべてのゲームオブジェクトを一括描画
	for (GameObject* obj : gameObjects_) {
		if (obj) {
			if (obj == player_) {
				if (!player_->IsDead() || player_->IsFalling()) {
					obj->Draw();
				}
			} else {
				obj->Draw();
			}
		}
	}

	// 3. 特殊演出（パーティクル）の描画
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	Model::PostDraw();

	// スプライト描画の開始
	Sprite::PreDraw(dxCommon->GetCommandList());

	// ★ 現在のフェーズ固有のスプライト描画処理を呼び出す（ポリモーフィズム）
	if (phaseState_) {
		phaseState_->Draw(this);
	}

	// 共通で常に描画するフェードスプライト
	if (fadeSprite_) {
		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
		fadeSprite_->Draw();
	}

	Sprite::PostDraw();
}

// ★ 状態を切り替える関数（古いswitch文ベースのChangePhaseを完全に置き換え）
void GameScene::ChangePhase(std::unique_ptr<GamePhaseState> newPhase) { phaseState_ = std::move(newPhase); }

// (GenerateBlocks と CheckAllCollision は変更なしのまま残す)
void GameScene::GenerateBlocks() { /* 省略 */ }
void GameScene::CheckAllCollision() { /* 省略 */ }