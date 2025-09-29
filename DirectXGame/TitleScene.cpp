#include "TitleScene.h"

TitleScene::~TitleScene() {}

void TitleScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	camera_.Initialize();

}

void TitleScene::Update() {

	if (input_->TriggerKey(DIK_SPACE)) {

		isFinished_ = true;
	}

}

void TitleScene::Draw() {}
