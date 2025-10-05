#include "TitleScene.h"

TitleScene::~TitleScene() {

	
	delete modelSkydome_;
	delete titleSkydome_;
	delete titleSprite_;

}

void TitleScene::Initialize() {

	spritePos_ = {300, -500};
	startPos_ = {325, 550};

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	camera_.Initialize();

	modelSkydome_ = Model::CreateFromOBJ("titleSkydome", true);

	titleSkydome_ = new TitleSkydome();
	titleSkydome_->Initialize(modelSkydome_, &camera_);

	titleTextureHandle_ = TextureManager::Load("titleSprite.png"); 
	titleSprite_ = Sprite::Create(titleTextureHandle_,spritePos_);
	titleSprite_->SetSize({600.0f, 300.0f});

	startTextureHandle_ = TextureManager::Load("startSprite.png");
	startSprite_ = Sprite::Create(startTextureHandle_, startPos_);

}

void TitleScene::Update() {

	if (input_->TriggerKey(DIK_SPACE)) {

		isFinished_ = true;
	}

	  spritePos_.y += 15.0f;
	titleSprite_->SetPosition(spritePos_);

	if (spritePos_.y > 50.0f) {
		spritePos_.y += (50.0f - spritePos_.y) * 0.1f;
	}

	startAlpha_ += alphaDir_;
	if (startAlpha_ <= 0.0f) {
		startAlpha_ = 0.0f;
		alphaDir_ *= -1; // 透明になったら増加方向に切替
	} else if (startAlpha_ >= 1.0f) {
		startAlpha_ = 1.0f;
		alphaDir_ *= -1; // 不透明になったら減少方向に切替
	}

	// 透明度を反映
	startSprite_->SetColor({1.0f, 1.0f, 1.0f, startAlpha_});

	titleSkydome_->Update();

}

void TitleScene::Draw() {

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw();

	titleSkydome_->Draw();

	Model::PostDraw();

	Sprite::PreDraw(commandList);

	titleSprite_->Draw();

	startSprite_->Draw();

	Sprite::PostDraw();

}
