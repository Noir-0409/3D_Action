#include "TitleScene.h"
#include "ConcreteTitleStates.h" // ★ 追加

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

	fadeSprite_ = Sprite::Create(0, {0, 0});
	fadeSprite_->SetSize({1920.0f, 1080.0f});

	// ★ 初期状態を「フェードイン状態」に設定
	fadeAlpha_ = 1.0f;
	fadeState_ = std::make_unique<TitleFadeInState>();

	fadeSprite_->SetColor({0.0f, 0.0f, 0.0f, fadeAlpha_});
}

void TitleScene::Update(float deltaTime) {
	// 累積時間を加算（点滅用）
	totalTime_ += deltaTime;

	// ★ フェード処理を現在の状態クラスに丸投げ（ポリモーフィズム）
	if (fadeState_) {
		fadeState_->Update(this, deltaTime);
	}

	// タイトルスプライトの滑らか移動
	float targetY = 90.0f;
	float easing = 0.1f;
	float diff = targetY - spritePos_.y;

	if (fabs(diff) > 0.1f) {
		spritePos_.y += diff * easing;
	}
	titleSprite_->SetPosition(spritePos_);

	// スタートスプライトの滑らか点滅
	float blinkSpeed = 0.5f;
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

// ★ 状態を切り替える関数
void TitleScene::ChangeState(std::unique_ptr<TitleFadeState> newState) { fadeState_ = std::move(newState); }