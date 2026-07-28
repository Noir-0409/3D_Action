#pragma once

namespace KamataEngine { // ★ namespace の開始

class Player;

class PlayerState {
public:
	virtual ~PlayerState() = default;
	virtual void Update(Player* player) = 0;
};

} // namespace KamataEngine