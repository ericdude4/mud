#include <iostream>
#include <cstdlib>
#include <time.h>
#include <string>
#include <vector>
#include "maze.h"

/*
Author: Eric Froese
Student Number: 5029954
Course: COSC 2P95
Assignment: 3
*/

namespace Maze {

	Maze::Maze(std::vector<Player::Player*> players) {	//default maze
		srand (time(NULL));
		this->players = players;
		this->wall = '#';
		this->newMaze();
	};

	Maze::~Maze() {
		for (int i = 0; i < this->players.size(); i++)
			delete[] this->players[i];
	}

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
		for (int i = 0; i < 400; i++) {
			this->maze[x][y] = ' ';
			if (rand() % 5 == 0) dir = rand() % 4;
			switch (dir) {
				case 0:	//EAST
					if (this->maze[x][y+1] != '#') y += 1;
					break;
				case 1:	//SOUTH
					if (this->maze[x+1][y] != '#') x += 1;
					break;
				case 2:	//WEST
					if (this->maze[x][y-1] != '#') y -= 1;
					break;
				case 3:	//NORTH
					if (this->maze[x-1][y] != '#') x -= 1;
					break;
			}
			if (rand() % 4 == 0) {	//25% chance of direction change
				if (dir == 1 || dir == 3) {	//if its vertical
					if (rand() % 2 == 0) {	//change is to a horiz
						dir = 0;
					} else dir = 2;
				}
				else if (dir == 2 || dir == 4) {	//if its horiz
					if (rand() % 4 == 0) {//less likely to go horiz to vert
						if (rand() % 2 == 0) {	//change is to a vertical
							dir = 1;
						} else dir = 3;
					}
				}
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

