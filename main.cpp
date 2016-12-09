#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "maze.cpp"

/*
Author: Eric Froese
Student Number: 5029954
Course: COSC 2P95
Assignment: 3
Note: for Dealers choice, the players have different travel behaviors based
on their personality (probability to change directions). Also, I think my maze
generation is good enough to qualify as well - as it prioritizes horizontal 
paths.
*/

static const int NUMTHREADS = 4;
float TIMESTEP_DELAY = 1.0;
const int seconds = 1000000;
volatile bool continuing = true;
volatile bool paused = false;
bool comments = false;
pthread_mutex_t lock;
Maze::Maze* m;
Player::Player* gold = new Player::Player('G', (std::string) "Gold", 0);
Player::Player* kobold = new Player::Player('K', (std::string) "Kobold", 0);
int main_pid;

void astalavista_baby(int signum)
{
	m->printMaze();
	for (int i = 0; i < 4; ++i)
	{
		std::cout << m->players[i]->name << "\tscore: " << m->players[i]->score << "\thealth: " << m->players[i]->health << std::endl;
	}
    std::cout <<"\nraises hand out of lava and presents a thumbs up... goodbye friend\nTERMNIATED" << std::endl;
	kill(main_pid, SIGTSTP);
}

void sigusr1(int signum)
{
    std::cout <<"SIGUSR1 : Displaying Maze and Score." << std::endl;
	m->printMaze();
	for (int i = 0; i < 4; ++i)
	{
		std::cout << m->players[i]->name << "\tscore: " << m->players[i]->score << "\thealth: " << m->players[i]->health << std::endl;
	}
}

void sigusr2(int signum)
{
    std::cout <<"SIGUSR2 : Resetting Maze" << std::endl;
    m->newMaze();
	m->printMaze();
	for (int i = 0; i < 4; ++i)
	{
		std::cout << m->players[i]->name << "\tscore: " << m->players[i]->score << "\thealth: " << m->players[i]->health << std::endl;
	}
	std::cout << "Process: " << getpid() << std::endl;
}

void interrupted(int sig) {
	paused = true;
	std::cout << "Reset map (1), resume (2), change timestep (3), exit (4) ";
	char choice;
	std::cin >> choice;
	switch (choice) {
		case '1':
			m->newMaze();
			break;
		case '2':
			break;
		case '3':
			std::cout << "enter new TIMESTEP_DELAY: ";
			std::cin >> TIMESTEP_DELAY;
			break;
		case '4':
			raise(SIGTERM);
	}
	paused = false;
}


int getNewDirection(int except) {
	int res;
	do {
		res = rand() % 4;
	} while (res == except);
	return res;
}

char checkForResource(int x, int y) {
	if (gold->x == x && gold->y == y) return 'G';
	else if (kobold->x == x && kobold->y == y) return 'K';
}

void newPosition(Player::Player* player) {
	int x, y;
	while(true) {
		x = rand() % 18 + 1;
		y = rand() % 78 + 1;
		if (m->maze[x][y] == ' ') {
			player->x = x;
			player->y = y;
			break;
		}
	}
}

void* run(void* my_player) {
	Player::Player* player;
	player = (Player::Player*) my_player;

	player->direction = rand() % 4;

	char resource;

	while (continuing) {
		while(paused) usleep(1000);

		if (player->health <= 0) {	//player is dead (waiting for respawn)
			player->health -= 1;
			if (player->health <= -10){
				player->reset();
				newPosition(player);
				if (comments) std::cout << player->name << " has respawned!" << std::endl;
			}
		}
		else if (player->fighting) {	//player is fighting
			pthread_mutex_lock(&lock);
			if (kobold->health > 0 && checkForResource(player->x, player->y) == 'K') { //make sure kobold isn't dead and hasnt respawned
				if (player->has_turn_to_attack) {
						int damage = rand() % 6 + 1;
						if (comments) std::cout << player->name << " attacks Kobold for " << damage << " damage! (" << kobold->health - damage << ")" << std::endl;
						kobold->health -= damage;
						if (kobold->health <= 0) {	//if two are fighting kobold, the one whos gets the last hit gets the points
							player->score += 25;
							if (comments) std::cout << player->name << " has slain Kobold!" << std::endl;
							kobold->reset();
							newPosition(kobold);
							player->fighting = false;
							player->has_turn_to_attack = true;
						} else player->has_turn_to_attack = false;
				} else {
					if (comments) std::cout << kobold->name << " attacks the " << player->name << " for 1 damage!" << std::endl;
					player->health -= 1;
					if (player->health <= 0) {
						if (comments) std::cout << player->name << " has been slain!" << std::endl;
					} else {
						player->has_turn_to_attack = true;
					}
				}
			} else {
				player->fighting = false;
				player->has_turn_to_attack = true;
			}
			pthread_mutex_unlock(&lock);
		}
		else {	//palyer is moving
			switch (player->direction) {
				case 0:	//EAST
					if (m->maze[player->x][player->y+1] != '#' && m->maze[player->x][player->y+1] != 'X') player->y += 1;
					else player->direction = getNewDirection(player->direction);
					break;
				case 1:	//SOUTH
					if (m->maze[player->x+1][player->y] != '#' && m->maze[player->x+1][player->y] != 'X') player->x += 1;
					else player->direction = getNewDirection(player->direction);
					break;
				case 2:	//WEST
					if (m->maze[player->x][player->y-1] != '#' && m->maze[player->x][player->y-1] != 'X') player->y -= 1;
					else player->direction = getNewDirection(player->direction);
					break;
				case 3:	//NORTH
					if (m->maze[player->x-1][player->y] != '#' && m->maze[player->x-1][player->y] != 'X') player->x -= 1;
					else player->direction = getNewDirection(player->direction);
					break;
			}
			//DEALERS CHOICE: PLAYERS CAN CHANGE DIRECTIONS WITHOUT HITTING WALLS
			if (rand() % player->personality == 0) player->direction = getNewDirection(player->direction);

			pthread_mutex_lock(&lock);
			resource = checkForResource(player->x, player->y);
			if (resource == 'G') {
				player->score += 10;
				if (comments) std::cout << player->name << " has struck gold!" << std::endl;
				newPosition(gold);
			}
			else if (resource == 'K') {
				player->fighting = true;
				if (comments) std::cout << player->name << " has entered battle!" << std::endl;
			}
			pthread_mutex_unlock(&lock);
		}
		usleep(TIMESTEP_DELAY * seconds);
	}
}

int main(int argc, char const *argv[])
{
	signal(SIGINT,interrupted);
	signal(SIGUSR1, sigusr1);
	signal(SIGUSR2, sigusr2);
	signal(SIGTERM, astalavista_baby);

	std::cout << "Enter TIMESTEP_DELAY seconds (float): ";
	std::cin >> TIMESTEP_DELAY;

	Player::Player* farvin = new Player::Player('1', (std::string) "Farvin", 3);
	Player::Player* garvin = new Player::Player('2', (std::string) "Garvin", 6);
	Player::Player* tharvin = new Player::Player('3', (std::string) "Tharvin", 9);
	Player::Player* sally = new Player::Player('4', (std::string) "Sally", 10);
	

	std::vector<Player::Player*> players;
	players.push_back(farvin);
	players.push_back(garvin);
	players.push_back(tharvin);
	players.push_back(sally);
	players.push_back(gold);
	players.push_back(kobold);

	m = new Maze::Maze(players);

	m->printMaze();
	for (int i = 0; i < 4; ++i)
	{
		std::cout << players[i]->name << "\tscore: " << players[i]->score << "\thealth: " << players[i]->health << std::endl;
	}
	main_pid = getpid();
	std::cout << "Main Process: " << main_pid << std::endl;

	pthread_t ct[NUMTHREADS];

	for (int i=0;i<NUMTHREADS;i++) {
		pthread_mutex_lock(&lock);
		pthread_create(&ct[i], NULL, &run, (void *)players[i]);
		pthread_mutex_unlock(&lock);
	}

	char sig;
	while(continuing) {
		std::cin >> sig;
		if (sig == 'd') raise(SIGUSR1);
		else if (sig == 'r') raise(SIGUSR2);
		usleep(1);
	}
	return 0;
}