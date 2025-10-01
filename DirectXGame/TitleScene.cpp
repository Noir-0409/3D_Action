#include "TitleScene.h"

TitleScene::~TitleScene() {

	
	delete modelSkydome_;
	delete titleSkydome_;

}

void TitleScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	camera_.Initialize();

	modelSkydome_ = Model::CreateFromOBJ("titleSkydome", true);

	titleSkydome_ = new TitleSkydome();
	titleSkydome_->Initialize(modelSkydome_, &camera_);

}

void TitleScene::Update() {

	if (input_->TriggerKey(DIK_SPACE)) {

		isFinished_ = true;
	}

	titleSkydome_->Update();

}

void TitleScene::Draw() {

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw();

	titleSkydome_->Draw();

	Model::PostDraw();

	Sprite::PreDraw(commandList);

	Sprite::PostDraw();

}
