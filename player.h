#include <iostream>
#include <string>

namespace Player {
	class Player {
	private:
	public:
		char ref;
		int x, y, health, score;
		std::string name;
		Player(char symbol, std::string name);
		~Player();//Destructor
	};
}