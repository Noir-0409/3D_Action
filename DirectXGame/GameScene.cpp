#include "GameScene.h"
#include "Enemy.h"
#include "Player.h"
#include "Skydome.h"

// ★すべての状態のインクルードを追加
#include "CountDownState.h"
#include "DeathState.h"
#include "GoalState.h"
#include "PlayerState.h"

GameScene::GameScene() { Initialize(); }

GameScene::~GameScene() {
	// オブジェクトの解放
	for (GameObject* obj : gameObjects_) {
		if (obj) {
			delete obj;
		}
	}
	gameObjects_.clear();

	// ★現在のステートの解放
	if (currentState_) {
		delete currentState_;
	}
}

void GameScene::Initialize() {

	// 古いオブジェクトの削除
	for (GameObject* obj : gameObjects_) {
		delete obj;
	}
	gameObjects_.clear();

	// ⭕【追加】もし古いマップが残っていたらここで個別に消してあげる
	if (mapChipField_) {
		delete mapChipField_;
		mapChipField_ = nullptr;
	}

	// 1. オブジェクトの生成
	Skydome* skydome = new Skydome();
	player_ = new Player();
	Enemy* enemy = new Enemy();
	mapChipField_ = new MapChipField(); // ★マップを生成

	// Player と Enemy にマップの場所を教えてあげる
	if (player_)
		player_->SetMapChipField(mapChipField_);
	if (enemy)
		enemy->SetMapChipField(mapChipField_);

	// 2. オブジェクトたちの初期化
	if (mapChipField_) {
		mapChipField_->Initialize();
		mapChipField_->LoadMapChipCSV("Resources/map.csv");
	}
	if (skydome)
		skydome->Initialize(modelSkydome_, camera_);
	if (player_)
		player_->Initialize(modelPlayer_, camera_, Vector3(0.0f, 0.0f, 0.0f));
	if (enemy)
		enemy->Initialize(modelEnemy_, camera_, Vector3(0.0f, 0.0f, 0.0f));

	// 3. 配列への登録（※ここにマップは入れない！）
	gameObjects_.push_back(skydome);
	gameObjects_.push_back(player_);
	gameObjects_.push_back(enemy);

	// ❌ mapChipField_ の push_back は消去するか、コメントアウトしてください！
	// gameObjects_.push_back(mapChipField_);

	// 4. 変数の初期化
	countdownTimer_ = 3.0f;
	gameTimer_ = 0.0f;
	startAlpha_ = 0.0f;
	deathAlpha_ = 0.0f;
	goalAlpha_ = 0.0f;

	// 最初の状態をセットする
	ChangeState(new CountDownState());
}

void GameScene::Update(float deltaTime) {

	// ⚡全てのゲームオブジェクト（プレイヤー、敵、背景）を一括更新！
	for (GameObject* obj : gameObjects_) {
		if (obj)
			obj->Update();
	}

	if (mapChipField_) {
		mapChipField_->Update();
	}

	// ⚡★【ステートパターン】面倒な分岐はすべて現在の状態クラスに丸投げ！
	if (currentState_) {
		currentState_->Update(this, deltaTime);
	}

}

void GameScene::Draw() {
	// 全てのオブジェクトを一括描画
	for (GameObject* obj : gameObjects_) {
		if (obj)
			obj->Draw();
	}

	// GameScene::Draw() の中
	for (GameObject* obj : gameObjects_) {
		if (obj)
			obj->Draw();
	}

	// ⭕【追加】個別でマップのDrawを呼んであげる
	if (mapChipField_) {
		mapChipField_->Draw();
	}

	// 現在の状態に応じた専用描画があれば実行
	if (currentState_) {
		currentState_->Draw(this);
	}

}

bool GameScene::IsFinished() const { return isFinished_; }

void GameScene::ChangeState(IGameState* newState) {
	// 以前の状態があればメモリから消す（メモリリーク防止）
	if (currentState_) {
		delete currentState_;
	}
	// 新しい状態にセット
	currentState_ = newState;
	// 新しい状態の「最初の1コマ（Enter）」を実行
	if (currentState_) {
		currentState_->Enter(this);
	}
}

// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// 各状態へのショートカット関数群
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void GameScene::ChangeStateToPlay() { ChangeState(new PlayState()); }
void GameScene::ChangeStateToDeath() { ChangeState(new DeathState()); }
void GameScene::ChangeStateToGoal() { ChangeState(new GoalState()); }

// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// 子クラスたちから頼まれる、ゲーム内の細かい判定や操作の代行
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void GameScene::SetPlayerInputEnabled(bool enabled) {

	enabled;

	if (player_) {
		// ※お使いのPlayerクラスに入力を無効化する関数（例: SetIsActive等）があれば、ここで呼び出してください
		// なければ、今のままで大丈夫です
	}
}

bool GameScene::CheckPlayerDeath() {
	if (!player_)
		return false;
	// プレイヤーが穴に落ちた、または体力が0になった等の条件を返す
	// 例: return player_->GetIsDead(); （あなたのコードの関数に合わせてね）
	return false;
}

bool GameScene::CheckPlayerGoal() {
	if (!player_)
		return false;
	// プレイヤーがゴール地点に到達したかの条件を返す
	// 例: return player_->GetIsGoal(); （あなたのコードの関数に合わせてね）
	return false;
}

bool GameScene::IsSpaceKeyPressed() {
	// ※ここでお使いのキー入力ライブラリ（Novice::CheckHitKeyなど）を使って、
	// スペースキーが押された瞬間（トリガー）かどうかを判定して返してください
	// 例: return Novice::CheckHitKey(DIK_SPACE);
	return false;
}

void GameScene::ResetGame() {
	Initialize(); // ゲームを最初からやり直す
}

void GameScene::BackToTitle() {
	// タイトルへ戻るフラグを立てるなど、元のmain.cppと連携する処理
	isFinished_ = true;
}