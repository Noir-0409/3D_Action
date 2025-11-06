#include "TitleScene.h"

TitleScene::~TitleScene() {
	delete modelSkydome_;
	delete titleSkydome_;
	delete titleSprite_;
	delete startSprite_;
	delete fadeSprite_;
}

void TitleScene::Initialize() {

	spritePos_ = {65, -500};
	startPos_ = {255, 500};
	totalTime_ = 0.0f;

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	camera_.Initialize();

	modelSkydome_ = Model::CreateFromOBJ("titleSkydome", true);

	titleSkydome_ = new TitleSkydome();
	titleSkydome_->Initialize(modelSkydome_, &camera_);

	titleTextureHandle_ = TextureManager::Load("titleSprite.png");
	titleSprite_ = Sprite::Create(titleTextureHandle_, spritePos_);

	startTextureHandle_ = TextureManager::Load("startSprite.png");
	startSprite_ = Sprite::Create(startTextureHandle_, startPos_);

	// フェードスプライト作成（黒、全画面）
	fadeSprite_ = Sprite::Create(0, {0, 0});
	fadeSprite_->SetSize({1920.0f, 1080.0f});
	fadeSprite_->SetColor({0.0f, 0.0f, 0.0f, fadeAlpha_});
}

void TitleScene::Update(float deltaTime) {

	// 累積時間を加算（点滅用）
	totalTime_ += deltaTime;

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
			fadeState_ = FadeState::None;
		}
	} else if (fadeState_ == FadeState::FadeOut) {
		fadeTimer_ += deltaTime;
		fadeAlpha_ = fadeTimer_ / fadeDuration_;
		if (fadeAlpha_ >= 1.0f) {
			fadeAlpha_ = 1.0f;
			isFinished_ = true;
		}
	}

	// タイトルスプライトの滑らか移動（減速付き）
	float targetY = 90.0f; // 止めたい高さ
	float easing = 0.1f;   // ゴールに近づくほど減速する割合
	float diff = targetY - spritePos_.y;

	if (fabs(diff) > 0.1f) {
		spritePos_.y += diff * easing;
	}
	titleSprite_->SetPosition(spritePos_);

	// スタートスプライトの滑らか点滅（正弦波で0～1のループ）
	float blinkSpeed = 0.5f; // 1秒で2回点滅
	startAlpha_ = (sinf(totalTime_ * blinkSpeed * 3.14159f * 2) + 1.0f) / 2.0f;
	startSprite_->SetColor({1.0f, 1.0f, 1.0f, startAlpha_});

	// スカイドーム更新
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
