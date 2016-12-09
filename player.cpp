#include "player.h"
#include <string>

namespace Player {

	Player::Player(char symbol, std::string name) {
		this->ref = symbol;
		this->health = symbol == 'K' ? 7 : 10;
		this->has_turn_to_attack = true;
		this->score = 0;
		this->name = name;
		this->fighting = false;
	}

	void Player::reset() {
		this->health = this->ref == 'K' ? 7 : 10;
		this->has_turn_to_attack = true;
		this->name = name;
		this->fighting = false;
		int x, y;
	}
}