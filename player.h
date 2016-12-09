#include <iostream>
#include <string>

/*
Author: Eric Froese
Student Number: 5029954
Course: COSC 2P95
Assignment: 3
*/

namespace Player {
	class Player {
	private:
	public:
		char ref;
		int x, y, health, score, direction, personality;
		bool fighting, has_turn_to_attack;
		std::string name;
		Player(char symbol, std::string name, int personality);
		~Player();//Destructor
		void reset();
	};
}