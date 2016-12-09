#include <iostream>
#include <string>

namespace Player {
	class Player {
	private:
	public:
		char ref;
		int x, y, health, score, direction;
		bool fighting, has_turn_to_attack;
		std::string name;
		Player(char symbol, std::string name);
		~Player();//Destructor
		void reset();
	};
}