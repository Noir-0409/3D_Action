#include "TitleScene.h"

TitleScene::~TitleScene() {
	delete modelSkydome_;
	delete titleSkydome_;
	delete titleSprite_;
	delete startSprite_;
	delete fadeSprite_;
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
	titleSprite_ = Sprite::Create(titleTextureHandle_, spritePos_);
	titleSprite_->SetSize({600.0f, 300.0f});

	startTextureHandle_ = TextureManager::Load("startSprite.png");
	startSprite_ = Sprite::Create(startTextureHandle_, startPos_);

	// フェードスプライト作成（黒、全画面）
	fadeSprite_ = Sprite::Create(0, {0, 0});
	fadeSprite_->SetSize({1920.0f, 1080.0f});
	fadeSprite_->SetColor({0.0f, 0.0f, 0.0f, fadeAlpha_});
}

void TitleScene::Update(float deltaTime) {

	// 入力はフェード中は無効
	if (fadeState_ == FadeState::None) {
		if (input_->TriggerKey(DIK_SPACE)) {
			fadeState_ = FadeState::FadeOut;
			fadeTimer_ = 0.0f;
		}
	}

	// フェード処理
	if (fadeState_ == FadeState::FadeIn) {
		fadeTimer_ += deltaTime;
		fadeAlpha_ = 1.0f - (fadeTimer_ / fadeDuration_);
		if (fadeAlpha_ <= 0.0f) {
			fadeAlpha_ = 0.0f;
			fadeState_ = FadeState::None; // フェードイン完了
		}
	} else if (fadeState_ == FadeState::FadeOut) {
		fadeTimer_ += deltaTime;
		fadeAlpha_ = fadeTimer_ / fadeDuration_;
		if (fadeAlpha_ >= 1.0f) {
			fadeAlpha_ = 1.0f;
			isFinished_ = true; // フェードアウト完了でシーン切替
		}
	}

	// 既存のスプライト処理
	spritePos_.y += 15.0f;
	titleSprite_->SetPosition(spritePos_);
	if (spritePos_.y > 50.0f) {
		spritePos_.y += (50.0f - spritePos_.y) * 0.1f;
	}

	startAlpha_ += alphaDir_;
	if (startAlpha_ <= 0.0f) {
		startAlpha_ = 0.0f;
		alphaDir_ *= -1;
	} else if (startAlpha_ >= 1.0f) {
		startAlpha_ = 1.0f;
		alphaDir_ *= -1;
	}
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

	// フェード描画
	fadeSprite_->SetColor({0.0f, 0.0f, 0.0f, fadeAlpha_});
	fadeSprite_->Draw();

	Sprite::PostDraw();
}
