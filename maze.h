#include <iostream>
#include <vector>
#include "player.cpp"

namespace Maze {
	class Maze {
	private:
		char wall;
		void makePath();
		void dropPlayers();
	public:
		char maze[20][80];
		std::vector<Player::Player*> players;
		Maze(std::vector<Player::Player*> players);
		~Maze();//Destructor

		void newMaze();
		void addPlayer();
		void printMaze();
		
	};
}