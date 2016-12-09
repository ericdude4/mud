#include "player.h"
#include <string>

/*
Author: Eric Froese
Student Number: 5029954
Course: COSC 2P95
Assignment: 3
*/

namespace Player {

	Player::Player(char symbol, std::string name, int personality) {
		this->ref = symbol;
		this->health = symbol == 'K' ? 7 : 10;
		this->has_turn_to_attack = true;
		this->score = 0;
		this->name = name;
		this->fighting = false;
		//personality = likelyhood to change direction while walking 
		//based on rand() % personality
		this->personality = personality;
	}

	Player::~Player() {}

	void Player::reset() {
		this->health = this->ref == 'K' ? 7 : 10;
		this->has_turn_to_attack = true;
		this->name = name;
		this->fighting = false;
		int x, y;
	}
}