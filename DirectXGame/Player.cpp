#define NOMINMAX

#include "Player.h"
#include "MapChipField.h"
#include "MathUtillity.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;

Player::~Player() {}

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	camera_ = camera;
	worldTransform_.rotation_.y = 3.14159f / 2.0f;
	input_ = Input::GetInstance();
}

void Player::Update() {
	worldTransform_.TransferMatrix();

	// --- 死亡落下中 ---
	if (isFalling_) {
		// Y方向の速度に重力を加算
		deathVelocityY_ += gravity_;
		worldTransform_.translation_.y += deathVelocityY_;

		// Z軸回転
		worldTransform_.rotation_.z += deathRotationSpeed_ * (1.0f / 60.0f); // 60FPS想定

		// 画面下まで落ちたら停止
		if (worldTransform_.translation_.y < -50.0f) {
			isFalling_ = false;
		}

		worldTransform_.UpdateMatrix();
		return; // 通常操作はしない
	}

	// --- 通常操作 ---
	if (inputEnabled_) {
		InputMove(); // velocity_ 計算
	} else {
		velocity_ = {0.0f, 0.0f, 0.0f};
	}

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	CheckMapCollision(collisionMapInfo);
	CollisionMove(collisionMapInfo);
	UpdateOnGround(collisionMapInfo);
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

// --- 死亡落下開始 ---
void Player::StartDeathFall() {
	isFalling_ = true;
	isDead_ = true;
	deathVelocityY_ = 0.25f; // 上にはねる初速
	worldTransform_.translation_.z -= 2.5f;
}

bool Player::IsOnIce() const {

	if (!onGround_ || mapChipField_ == nullptr) {
		return false;
	}

	Vector3 footPos = worldTransform_.translation_;
	footPos.y -= kHeight / 2.0f + 0.02f;

	auto index = mapChipField_->GetMapChipIndexSetByPosition(footPos);
	MapChipType type = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);

	return type == MapChipType::kIce;
}

void Player::InputMove() {

	Vector3 acceleration{};

	bool onIce = IsOnIce();

	float accel = onIce ? kIceAcceleration : kAcceleration;
	float attenuation = onIce ? kIceAttenuation : kAttenuation;

	// 右移動
	if (input_->PushKey(DIK_D)) {
		if (velocity_.x < 0.0f) {
			velocity_.x *= (1.0f - attenuation);
		}
		acceleration.x += accel;
		lrDirection_ = LRDirection::kRight;
	}
	// 左移動
	else if (input_->PushKey(DIK_A)) {
		if (velocity_.x > 0.0f) {
			velocity_.x *= (1.0f - attenuation);
		}
		acceleration.x -= accel;
		lrDirection_ = LRDirection::kLeft;
	}
	// 入力なし
	/*else {
		velocity_.x *= (1.0f - attenuation);
	}*/

	else {
		if (!onIce) {
			velocity_.x *= (1.0f - attenuation);
		}
	}

	velocity_ += acceleration;
	//velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	float maxSpeed = onIce ? kIceMaxSpeed : kLimitRunSpeed;
	velocity_.x = std::clamp(velocity_.x, -maxSpeed, maxSpeed);

	// --- ジャンプ処理（そのまま） ---
	if (input_->PushKey(DIK_W)) {
		if (onGround_) {
			velocity_.y = kJumpAcceleration;
			onGround_ = false;
			jumpTime_ = 0.0f;
		} else if (jumpTime_ < kMaxJumpTime) {
			velocity_.y = kJumpAcceleration;
		}
		jumpTime_ += 1.0f / 60.0f;
	} else {
		jumpTime_ = kMaxJumpTime;
	}

	// --- 重力 ---
	if (!onGround_) {
		if (velocity_.y > 0.0f) {
			velocity_.y -= kGravityAcceleration * 0.6f;
		} else {
			velocity_.y -= kGravityAcceleration * 0.8f;
		}
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	UpdateRotation();
}

void Player::CollisionMove(const CollisionMapInfo& info) { worldTransform_.translation_ += info.move; }

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	if (info.move.y >= 0.0f)
		return;

	std::array<Vector3, kNumCorner> positionsNew;
	Vector3 center = worldTransform_.translation_ + info.move;

	for (uint32_t i = 0; i < positionsNew.size(); ++i)
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));

	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;
	bool hit = false;

	// 左下チェック
	Vector3 checkPosL = positionsNew[kLeftBottom];
	checkPosL.y -= kGroundSearchHeight;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(checkPosL);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kDamage)
		isDead_ = true;
	else if (mapChipType == MapChipType::kGoal)
		isGoal_ = true;

	else if (mapChipType == MapChipType::kBlock || mapChipType == MapChipType::kIce)
		hit = true;


	// 右下チェック
	Vector3 checkPosR = positionsNew[kRightBottom];
	checkPosR.y -= kGroundSearchHeight;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(checkPosR);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kDamage)
		isDead_ = true;
	else if (mapChipType == MapChipType::kGoal)
		isGoal_ = true;
	
	else if (mapChipType == MapChipType::kBlock || mapChipType == MapChipType::kIce)
		hit = true;


	if (hit) {
		Vector3 bottomPos = worldTransform_.translation_;
		bottomPos.y -= kHeight / 2.0f;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(bottomPos);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + kHeight / 2.0f + kBlank);
		info.landing = true;
	} else {
		info.landing = false;
	}
}

// 上方向チェック
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	if (info.move.y <= 0.0f)
		return;

	std::array<Vector3, kNumCorner> positionsNew;
	Vector3 center = worldTransform_.translation_ + info.move;

	for (uint32_t i = 0; i < positionsNew.size(); ++i)
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));

	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;
	bool hit = false;

	// 左上チェック
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDamage)
		isDead_ = true;
	else if (mapChipType == MapChipType::kGoal)
		isGoal_ = true;
	else if (mapChipType == MapChipType::kBlock)
		hit = true;

	// 右上チェック
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDamage)
		isDead_ = true;
	else if (mapChipType == MapChipType::kGoal)
		isGoal_ = true;
	else if (mapChipType == MapChipType::kBlock)
		hit = true;

	if (hit) {
		Vector3 pos = worldTransform_.translation_;
		pos.y += kHeight / 2.0f;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		info.ceiling = true;
		velocity_.y = 0.0f;
	}
}

// 左右方向チェック（ゴール判定追加済み）
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0.0f)
		return;

	std::array<Vector3, kNumCorner> positionsNew;
	Vector3 center = worldTransform_.translation_ + info.move;
	for (uint32_t i = 0; i < positionsNew.size(); ++i)
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));

	float playerLeft = worldTransform_.translation_.x - kWidth / 2.0f;
	float maxDeltaX = info.move.x;

	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;
	MapChipField::Rect rect;

	// 左上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDamage)
		isDead_ = true;
	else if (mapChipType == MapChipType::kGoal)
		isGoal_ = true;
	else if (mapChipType == MapChipType::kBlock) {
		rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float deltaX = rect.right - playerLeft;
		if (deltaX > maxDeltaX)
			maxDeltaX = deltaX;
	}

	// 左下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDamage)
		isDead_ = true;
	else if (mapChipType == MapChipType::kGoal)
		isGoal_ = true;
	else if (mapChipType == MapChipType::kBlock) {
		rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float deltaX = rect.right - playerLeft;
		if (deltaX > maxDeltaX)
			maxDeltaX = deltaX;
	}

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
	Vector3 center = worldTransform_.translation_ + info.move;
	for (uint32_t i = 0; i < positionsNew.size(); ++i)
		positionsNew[i] = CornerPosition(center, static_cast<Corner>(i));

	float playerRight = worldTransform_.translation_.x + kWidth / 2.0f;
	float minDeltaX = info.move.x;

	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;
	MapChipField::Rect rect;

	// 右上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDamage)
		isDead_ = true;
	else if (mapChipType == MapChipType::kGoal)
		isGoal_ = true;
	else if (mapChipType == MapChipType::kBlock) {
		rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float deltaX = rect.left - playerRight;
		if (deltaX < minDeltaX)
			minDeltaX = deltaX;
	}

	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDamage)
		isDead_ = true;
	else if (mapChipType == MapChipType::kGoal)
		isGoal_ = true;
	else if (mapChipType == MapChipType::kBlock) {
		rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float deltaX = rect.left - playerRight;
		if (deltaX < minDeltaX)
			minDeltaX = deltaX;
	}

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
		//velocity_.x *= (1.0f - kAttenuationLanding);

		if (!IsOnIce()) {
			velocity_.x *= (1.0f - kAttenuationLanding);
		}


	} else if (velocity_.y > 0.0f) {
		// 上昇中は接地解除
		onGround_ = false;
	} else {
		// 下端の下にブロックがあるか確認して接地
		Vector3 bottomPos = worldTransform_.translation_;
		bottomPos.y -= kHeight / 2.0f + 0.01f; // 少し下に補正
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(bottomPos);
		MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		//onGround_ = (mapChipType == MapChipType::kBlock);
		onGround_ = (mapChipType == MapChipType::kBlock || mapChipType == MapChipType::kIce);

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
