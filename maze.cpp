#include <iostream>
#include <cstdlib>
#include <time.h>
#include <string>
#include <vector>

#include "maze.h"

namespace Maze {

	Maze::Maze(std::vector<Player::Player*> players) {	//default maze
		srand (time(NULL));
		this->players = players;
		this->wall = '#';
		this->newMaze();
		this->printMaze();
	};

	void Maze::newMaze() {
		//initialize the map with a border

		for (int i = 0; i < 80; i++) {
			this->maze[0][i] = wall;
			this->maze[19][i] = wall;
		}
		for (int i = 1; i < 19; i++) {
			this->maze[i][0] = wall;
			for (int j = 1; j < 80; j++)
				this->maze[i][j] = 'X';
			this->maze[i][79] = wall;
		}
		this->makePath();
		this->dropPlayers();
	};

	void Maze::makePath() {

		int x = rand() % 18 + 1;
		int y = rand() % 78 + 1;
		int dir = 0;
		for (int i = 0; i < 2000; i++) {
			this->maze[x][y] = ' ';
			if (rand() % 4 == 0) dir = rand() % 4;
			switch (dir) {
				case 0:	//NORTH
					if (this->maze[x][y+1] != '#') y += 1;
					break;
				case 1:	//EAST
					if (this->maze[x+1][y] != '#') x += 1;
					break;
				case 2:	//SOUTH
					if (this->maze[x][y-1] != '#') y -= 1;
					break;
				case 3:	//WEST
					if (this->maze[x-1][y] != '#') x -= 1;
					break;
			}
			if (i % 100 == 0) {	//every 100 itterations, randomize x and y again
				x = rand() % 18 + 1;
				y = rand() % 78 + 1;
			}
		}
	};

	void Maze::dropPlayers() {	//place the players on the map
		int x, y;
		for (int i = 0; i < this->players.size(); i++) {
			while(true) {
				x = rand() % 18 + 1;
				y = rand() % 78 + 1;
				if (this->maze[x][y] == ' ') {
					this->players[i]->x = x;
					this->players[i]->y = y;
					this->maze[x][y] == 'D'; //temporarily disable this space
					break;
				}
			}
		}
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 80; j++) {
				if (this->maze[i][j] == 'D')
					this->maze[i][j] == ' ';
			}
		}
	};

	void Maze::printMaze() {
		bool player_found = false;
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 80; j++) {
				if (this->maze[i][j] == ' ') {
					for (int k = 0; k < this->players.size(); k++) {
						if (this->players[k]->x == i && this->players[k]->y == j){
							std::cout << this->players[k]->ref;
							player_found = true;
							break;
						}
					}
					if (!player_found) std::cout << this->maze[i][j];
					player_found = false;
				}
				else std::cout << this->maze[i][j];
			}
			std::cout << std::endl;
		}
	}
}

