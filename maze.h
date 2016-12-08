#include <iostream>
#include <vector>
#include "player.cpp"

namespace Maze {
	class Maze {
	private:
		char wall;
		char maze[20][80];
		std::vector<Player::Player*> players;
		void makePath();
		void dropPlayers();
	public:
		Maze(std::vector<Player::Player*> players);
		~Maze();//Destructor

		void newMaze();
		void addPlayer();
		void printMaze();
		
	};
}