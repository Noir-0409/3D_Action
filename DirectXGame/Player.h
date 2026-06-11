#pragma once
#include "AABB.h"
#include "GameObject.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class MapChipField;
class Enemy;

enum class LRDirection {
	kRight,
	kLeft,
};

/// <summary>
/// playerを管理するクラス（GameObjectを継承）
/// </summary>
class Player : public GameObject { // ★「: public GameObject」を追加して継承させる

public:
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitwall = false;
		Vector3 move;
	};

	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,
		kNumCorner // 要素数
	};

	// ★「override」を追加。親クラスの仮想デストラクタと連動させます
	~Player() override;

	// ★ 引数を「const Camera* camera」に変更し、「override」を追加
	void Initialize(Model* model, const Camera* camera, const Vector3& position) override;

	// ★「override」を追加
	void Update() override;

	// ★ 引数を「Camera& camera」から「GameObject」のルールに合わせて一新、または独自仕様を残すため
	// 今回は基底クラスのDrawを上書き（引数なし）にするか、カメラを渡す形にするかですが、
	// すでにメンバ変数に「camera_」を持っているので、引数なしの Draw() override に統一できます！
	// GameScene.cpp 側では player_->Draw() ではなく、一括ループまたはそのまま引数なしで呼べるようになります。
	void Draw() override;

	// --- 以下、Player独自の関数はすべて「そのまま」で大丈夫です！ ---

	// 移動速度を取得
	const Vector3& GetVelocity() const { return velocity_; }

	// 初期位置を指定
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 移動処理
	void InputMove();

	// 移動速度を加算
	void CollisionMove(const CollisionMapInfo& info);

	// 各方向の当たり判定
	void CheckMapCollision(CollisionMapInfo& info);

	// 上方向の当たり判定
	void CheckMapCollisionUp(CollisionMapInfo& info);

	// 下方向の当たり判定
	void CheckMapCollisionDown(CollisionMapInfo& info);

	// 左方向の当たり判定
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	// 右方向の当たり判定
	void CheckMapCollisionRight(CollisionMapInfo& info);

	// プレイヤーの4頂点
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	// プレイヤーが地面に接触しているかどうか
	void UpdateOnGround(const CollisionMapInfo& info);

	// 方向転換時の回転
	void UpdateRotation();

	// 回転をスムーズに行う
	void UpdateRotationSmooth();

	// 壁に当たっているか
	void UpdateHitWall(const CollisionMapInfo& info);

	// 立方体の当たり判定
	AABB GetAABB();

	// 敵との当たり判定
	void OnCollision(const Enemy* enemy);

	bool inputEnabled_ = true;

	// シーン毎にキー入力の有効/無効を切り替える
	void SetInputEnabled(bool enabled) { inputEnabled_ = enabled; }

	// ゴールしたかどうか
	bool IsGoal() const { return isGoal_; }

	// ミス時の落下演出開始
	void StartDeathFall();

	// 実際に落下させる
	bool IsFalling() const { return isFalling_; }

	// 氷ブロックの上にいるか
	bool IsOnIce() const;

	bool wasMovingInput_ = false; // 前フレームで左右入力があったか

private:

	Input* input_ = nullptr;

	Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.003f;
	static inline const float kLimitRunSpeed = 0.5f;
	static inline const float kAttenuation = 0.1f;

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;

	static inline const float kGravityAcceleration = 0.03f;
	static inline const float kLimitFallSpeed = 1.0f;
	static inline const float kJumpAcceleration = 0.3f;

	MapChipField* mapChipField_ = nullptr;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.04f;

	static inline const float kAttenuationLanding = 0.0f;

	static inline const float kGroundSearchHeight = 0.01f;

	static inline const float kAttenuationWall = 0.00001f;

	bool isAttacking_ = false;
	int attackTimer_ = 0;

	float jumpTime_ = 0.0f;
	static inline const float kMaxJumpTime = 0.25f;

	bool isGoal_ = false;

	bool isFalling_ = false;
	Vector3 deathFallVelocity_ = {0.0f, -5.0f, 0.0f};
	float deathRotationSpeed_ = 180.0f;

	float deathVelocityY_ = 0.0f; // Y方向速度
	const float gravity_ = -0.01f;

	// Ice 用
	static inline const float kIceAcceleration = 0.006f;
	static inline const float kIceAttenuation = 0.001f;
	static inline const float kIceMaxSpeed = 0.8f;
};