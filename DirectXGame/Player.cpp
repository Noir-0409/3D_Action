#define NOMINMAX

#include "Player.h"
#include "MapChipField.h"
#include "MathUtillity.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;

Player::~Player() {}

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {

	//assert(model);

	model_ = model;

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	camera;

	worldTransform_.rotation_.y = 3.14159f / 2.0f;

	input_ = Input::GetInstance();
}

void Player::Update() {

	worldTransform_.TransferMatrix();

	  if (inputEnabled_) {
		InputMove(); // velocity_ 計算
	} else {
		velocity_ = {0.0f, 0.0f, 0.0f}; // 念のため移動速度はゼロに
	}

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	CheckMapCollision(collisionMapInfo); // move を補正
	CollisionMove(collisionMapInfo);     // 実際に位置を更新
	UpdateOnGround(collisionMapInfo);    // onGround 更新
	UpdateHitWall(collisionMapInfo);

	if (isAttacking_) {
		--attackTimer_;
		if (attackTimer_ <= 0) {
			isAttacking_ = false;
		}
	}

	worldTransform_.UpdateMatrix();
	worldTransform_.TransferMatrix();
}

void Player::Draw(Camera& camera) { model_->Draw(worldTransform_, camera); }

void Player::InputMove() {
	Vector3 acceleration{};

	// 左右移動
	if (Input::GetInstance()->PushKey(DIK_D)) {
		if (velocity_.x < 0.0f)
			velocity_.x *= (1.0f - kAttenuation);
		acceleration.x += kAcceleration;
		lrDirection_ = LRDirection::kRight;
	} else if (Input::GetInstance()->PushKey(DIK_A)) {
		if (velocity_.x > 0.0f)
			velocity_.x *= (1.0f - kAttenuation);
		acceleration.x -= kAcceleration;
		lrDirection_ = LRDirection::kLeft;
	} else {
		velocity_.x *= (1.0f - kAttenuation);
	}

	velocity_ += acceleration;
	velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	// ジャンプ判定：壁に触れていても接地していれば可能
	if (Input::GetInstance()->PushKey(DIK_W)) {
		if (onGround_) {
			velocity_.y = kJumpAcceleration;
			onGround_ = false;
		}
	}

	// 空中重力処理
	if (!onGround_) {
		velocity_.y -= kGravityAcceleration;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	// 回転更新
	UpdateRotation();

	// 移動自体は CollisionMove で行う
}

void Player::CollisionMove(const CollisionMapInfo& info) { worldTransform_.translation_ += info.move; }

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {

	if (info.move.y <= 0) {

		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	// 移動後の中心座標を計算
	Vector3 center;
	center.x = worldTransform_.translation_.x + info.move.x;
	center.y = worldTransform_.translation_.y + info.move.y;
	center.z = worldTransform_.translation_.z + info.move.z;

	// 各コーナーの位置を計算
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	bool hit = false;

	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {

		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {

		hit = true;
	}

	if (hit) {

		Vector3 pos = worldTransform_.translation_;
		pos.y += kHeight / 2.0f;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);

		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));

		info.ceiling = true;
	}

	if (info.ceiling) {

		DebugText::GetInstance()->ConsolePrintf("hit\n");

		velocity_.y = 0;
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	if (info.move.y >= 0.0f)
		return; // 上方向なら無視

	std::array<Vector3, kNumCorner> positionsNew;

	// 移動後の中心座標
	Vector3 center;
	center.x = worldTransform_.translation_.x + info.move.x;
	center.y = worldTransform_.translation_.y + info.move.y;
	center.z = worldTransform_.translation_.z + info.move.z;

	// コーナー座標を計算
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));
	}

	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;
	bool hit = false;

	// 左下チェック
	Vector3 checkPosL = positionsNew[kLeftBottom];
	checkPosL.y -= kGroundSearchHeight;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(checkPosL);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock)
		hit = true;

	// 右下チェック
	Vector3 checkPosR = positionsNew[kRightBottom];
	checkPosR.y -= kGroundSearchHeight;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(checkPosR);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock)
		hit = true;

	if (hit) {
		// 衝突しているブロックの矩形を取得
		Vector3 bottomPos = worldTransform_.translation_;
		bottomPos.y -= kHeight / 2.0f;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(bottomPos);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		// 移動量をブロック上に補正
		info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + kHeight / 2.0f + kBlank);

		// 着地フラグ
		info.landing = true;
	} else {
		info.landing = false;
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0.0f)
		return; // 左方向に動いていなければ無視

	std::array<Vector3, kNumCorner> positionsNew;

	Vector3 center;
	center.x = worldTransform_.translation_.x + info.move.x;
	center.y = worldTransform_.translation_.y + info.move.y;
	center.z = worldTransform_.translation_.z + info.move.z;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));
	}

	float playerLeft = worldTransform_.translation_.x - kWidth / 2.0f;
	float maxDeltaX = info.move.x; // 左方向なので最大値として初期化

	// 左上チェック
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
		float deltaX = rect.right - playerLeft; // 左端がブロック右端を超えないように
		if (deltaX > maxDeltaX)
			maxDeltaX = deltaX;
	}

	// 左下チェック
	Vector3 checkPosL = positionsNew[kLeftBottom];
	checkPosL.y -= kGroundSearchHeight;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(checkPosL);
	rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
		float deltaX = rect.right - playerLeft;
		if (deltaX > maxDeltaX)
			maxDeltaX = deltaX;
	}

	// 補正
	if (maxDeltaX > info.move.x) {
		info.move.x = maxDeltaX;
		velocity_.x = 0.0f;
		info.hitwall = true;
	}
}


void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	if (info.move.x <= 0.0f)
		return;

	std::array<Vector3, kNumCorner> positionsNew;

	Vector3 center;
		center.x = worldTransform_.translation_.x + info.move.x;
		center.y = worldTransform_.translation_.y + info.move.y;
		center.z = worldTransform_.translation_.z + info.move.z;
	

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));
	}

	float playerRight = worldTransform_.translation_.x + kWidth / 2.0f;
	float minDeltaX = info.move.x;

	// 右上チェック
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
		float deltaX = rect.left - playerRight;
		if (deltaX < minDeltaX)
			minDeltaX = deltaX;
	}

	// 右下チェック
	Vector3 checkPosR = positionsNew[kRightBottom];
	checkPosR.y -= kGroundSearchHeight;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(checkPosR);
	rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
		float deltaX = rect.left - playerRight;
		if (deltaX < minDeltaX)
			minDeltaX = deltaX;
	}

	// 補正
	if (minDeltaX < info.move.x) {
		info.move.x = minDeltaX;
		velocity_.x = 0.0f;
		info.hitwall = true;
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {

	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0},
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0},
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0},
	};

	return Vector3(center.x + offsetTable[static_cast<uint32_t>(corner)].x, center.y + offsetTable[static_cast<uint32_t>(corner)].y, center.z + offsetTable[static_cast<uint32_t>(corner)].z);
}

void Player::UpdateOnGround(const CollisionMapInfo& info) {

	// 接地フラグを更新
	if (info.landing) {
		onGround_ = true;
		velocity_.y = 0.0f;
		velocity_.x *= (1.0f - kAttenuationLanding);
	} else if (velocity_.y > 0.0f) {
		// 上昇中は接地解除
		onGround_ = false;
	} else {
		// 下端の下にブロックがあるか確認して接地
		Vector3 bottomPos = worldTransform_.translation_;
		bottomPos.y -= kHeight / 2.0f + 0.01f; // 少し下に補正
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(bottomPos);
		MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		onGround_ = (mapChipType == MapChipType::kBlock);
	}
}

void Player::UpdateRotation() {

	float targetRotationY = (lrDirection_ == LRDirection::kRight) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;

	float currentY = worldTransform_.rotation_.y;

	// 角度差を -π～π に正規化
	float delta = std::fmod(targetRotationY - currentY + std::numbers::pi_v<float>, 2.0f * std::numbers::pi_v<float>) - std::numbers::pi_v<float>;

	float rotationSpeed = 0.2f; // 回転スピード
	worldTransform_.rotation_.y += delta * rotationSpeed;
}

void Player::UpdateRotationSmooth() {}

void Player::UpdateHitWall(const CollisionMapInfo& info) {

	if (info.hitwall) {
	
	velocity_.x *= (1.0f - kAttenuationWall);
	
	}

}

Vector3 Player::GetWorldPosition() const{

	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得
	// ワールド行列のTx
	worldPos.x = worldTransform_.translation_.x;

	// ワールド行列のTy
	worldPos.y = worldTransform_.translation_.y;

	// ワールド行列のTz
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

AABB Player::GetAABB() { 

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};

	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;

}

void Player::OnCollision(const Enemy* enemy) {

	(void)enemy;
	isDead_ = true;

}

void Player::Attack() {

	 if (!isAttacking_) { // 攻撃中でなければ
		isAttacking_ = true;
		attackTimer_ = 10; // 攻撃の持続フレーム（例：10フレーム）
	}
	
}

AABB Player::GetAttackAABB() const { 

	  AABB attackBox;

	// 攻撃範囲のサイズ
	float attackWidth = 1.0f;  // 横幅
	float attackHeight = 0.8f; // 高さ

	// プレイヤーの中心座標
	Vector3 center = GetWorldPosition();

	Vector3 offset = {attackWidth / 2.0f, attackHeight / 2.0f, 0.0f};

	// min
	attackBox.min.x = center.x + offset.x - attackWidth / 2.0f;
	attackBox.min.y = center.y + offset.y - attackHeight / 2.0f;
	attackBox.min.z = center.z + offset.z - 0.0f;

	// max
	attackBox.max.x = center.x + offset.x + attackWidth / 2.0f;
	attackBox.max.y = center.y + offset.y + attackHeight / 2.0f;
	attackBox.max.z = center.z + offset.z + 0.0f;

	return attackBox;

}
