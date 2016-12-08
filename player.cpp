#include "player.h"
#include <string>

namespace Player {

	Player::Player(char symbol, std::string name) {
		this->ref = symbol;
		this->health = 10;
		this->score = 0;
		this->name = name;
	}
}