#pragma once

class Player;

class PlayerState {
public:
	virtual ~PlayerState() = default;
	virtual void Update(Player* player) = 0;
};