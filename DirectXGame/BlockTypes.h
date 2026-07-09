#pragma once

#include "BaseBlock.h"
#include "MapChipField.h"
#include <functional>
#include <memory> // ★ std::unique_ptr を使用するために追加

class NormalBlock : public BaseBlock {
public:
	using BaseBlock::BaseBlock; // 親のコンストラクタが std::unique_ptr になったので自動で追従します
	void Draw(const KamataEngine::Camera& camera) override {
		if (model_ && worldTransform_) {
			model_->Draw(*worldTransform_, camera);
		}
	}
};

class FireBlock : public BaseBlock {
private:
	const bool& fireToggle_;
	uint32_t tex1_;
	uint32_t tex2_;

public:
	// ⭕ 引数の wt を std::unique_ptr に変更し、BaseBlock へ std::move(wt) で渡す
	FireBlock(KamataEngine::Model* model, std::unique_ptr<KamataEngine::WorldTransform> wt, const bool& toggle, uint32_t t1, uint32_t t2)
	    : BaseBlock(model, std::move(wt)), fireToggle_(toggle), tex1_(t1), tex2_(t2) {}

	void Draw(const KamataEngine::Camera& camera) override {
		if (model_ && worldTransform_) {
			uint32_t currentTex = fireToggle_ ? tex1_ : tex2_;
			model_->Draw(*worldTransform_, camera, currentTex);
		}
	}
};

class SwitchBlock : public BaseBlock {
private:
	std::function<MapChipType()> getTypeFunc_;     // 現在のマップの状態を取得する関数
	MapChipType targetType_;                       // 自分が本来何のブロックか（kRed か kBlue）
	KamataEngine::Model* modelVanished_ = nullptr; // 消滅時（半透明）のモデル

public:
	// ⭕ 引数の wt を std::unique_ptr に変更し、BaseBlock へ std::move(wt) で渡す
	SwitchBlock(
	    KamataEngine::Model* modelNormal, KamataEngine::Model* modelVanished, std::unique_ptr<KamataEngine::WorldTransform> wt, std::function<MapChipType()> getTypeFunc, MapChipType targetType)
	    : BaseBlock(modelNormal, std::move(wt)), modelVanished_(modelVanished), getTypeFunc_(getTypeFunc), targetType_(targetType) {}

	void Draw(const KamataEngine::Camera& camera) override {
		if (!worldTransform_)
			return;

		// 現在のマップの状態を取得
		MapChipType currentType = getTypeFunc_();

		if (currentType == targetType_) {
			// 通常時（不透明）の描画
			if (model_)
				model_->Draw(*worldTransform_, camera);
		} else if (currentType == MapChipType::kBlank) {
			// 消滅中（半透明）の描画
			if (modelVanished_)
				modelVanished_->Draw(*worldTransform_, camera);
		}
	}
};