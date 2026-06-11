#pragma once

class TitleScene;

class TitleFadeState {
public:
	virtual ~TitleFadeState() = default;
	virtual void Update(TitleScene* scene, float deltaTime) = 0;
};