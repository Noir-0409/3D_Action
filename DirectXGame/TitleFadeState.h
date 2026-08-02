#pragma once

namespace KamataEngine { // ★ namespace の開始

class TitleScene;

class TitleFadeState {
public:
	virtual ~TitleFadeState() = default;
	virtual void Update(TitleScene* scene, float deltaTime) = 0;
};

} // namespace KamataEngine