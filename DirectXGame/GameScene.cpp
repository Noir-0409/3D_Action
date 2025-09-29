#include "GameScene.h"

GameScene::~GameScene() {

	delete modelPlayer_;
	delete player_;
	delete mapChipField_;
	delete enemy_;
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

	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCSV("Resources/map.csv");

	camera_.Initialize();

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(5, 18);
	enemy_ = new Enemy();
	enemy_->Initialize(modelEnemy_, &camera_, enemyPosition);
	enemy_->SetMapChipField(mapChipField_);

	cameraController_ = new CameraController();
	cameraController_->SetCamera(&camera_);
	cameraController_->SetTarget(player_);
	cameraController_->Initialize();
	cameraController_->Reset();

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	input_ = Input::GetInstance();

	worldTransform_.Initialize();

	GenerateBlocks();


}

void GameScene::Update() {
	
	player_->Update(); 
	enemy_->Update();

for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			// アフィン変換行列の作成
			worldTransformBlockYoko->UpdateMatrix();
		}
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
	enemy_->Draw();
	skydome_->Draw();

	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());

	Sprite::PostDraw();
}

void GameScene::GenerateBlocks() {

	//uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	//uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

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

