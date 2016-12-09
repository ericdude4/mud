#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "maze.cpp"

static const int NUMTHREADS = 4;
const int TIMESTEP_DELAY = 1000;
const int seconds = 1000000;
volatile bool continuing = true;
pthread_mutex_t lock;
Maze::Maze* m;
Player::Player* gold = new Player::Player('G', (std::string) "Gold");
Player::Player* kobold = new Player::Player('K', (std::string) "Kobold");

void interrupted(int sig) {
	std::cout<<"\nComputations complete."<<std::endl;
	continuing=false;
}


int getNewDirection(int except, std::string name) {
	int res;
	do {
		res = rand() % 4;
	} while (res == except);
	//std::cout << "new direction: " << res << " for " << name << std::endl;
	return res;
}

char checkForResource(int x, int y) {
	// pthread_mutex_lock(&lock);
	if (gold->x == x && gold->y == y) return 'G';
	else if (kobold->x == x && kobold->y == y) return 'K';
	// pthread_mutex_unlock(&lock);
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
		// pthread_mutex_lock(&lock);
		if (player->health <= 0) {	//player is dead (waiting for respawn)
			player->health -= 1;
			if (player->health <= -10){
				player->reset();
				newPosition(player);
				std::cout << player->name << " has respawned!" << std::endl;
			}
		}
		else if (player->fighting) {	//player is fighting
			pthread_mutex_lock(&lock);
			if (kobold->health > 0 && checkForResource(player->x, player->y) == 'K') { //make sure kobold isn't dead and hasnt respawned
				if (player->has_turn_to_attack) {
						int damage = rand() % 6 + 1;
						std::cout << player->name << " attacks Kobold for " << damage << " damage! (" << kobold->health - damage << ")" << std::endl;
						kobold->health -= damage;
						if (kobold->health <= 0) {	//if two are fighting kobold, the one whos gets the last hit gets the points
							player->score += 25;
							std::cout << player->name << " has slain Kobold!" << std::endl;
							kobold->reset();
							newPosition(kobold);
							player->fighting = false;
							player->has_turn_to_attack = true;
						} else player->has_turn_to_attack = false;
				} else {
					std::cout << kobold->name << " attacks the " << player->name << " for 1 damage!" << std::endl;
					player->health -= 10;
					if (player->health <= 0) {
						std::cout << player->name << " has been slain!" << std::endl;
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
					else player->direction = getNewDirection(player->direction, player->name);
					break;
				case 1:	//SOUTH
					if (m->maze[player->x+1][player->y] != '#' && m->maze[player->x+1][player->y] != 'X') player->x += 1;
					else player->direction = getNewDirection(player->direction, player->name);
					break;
				case 2:	//WEST
					if (m->maze[player->x][player->y-1] != '#' && m->maze[player->x][player->y-1] != 'X') player->y -= 1;
					else player->direction = getNewDirection(player->direction, player->name);
					break;
				case 3:	//NORTH
					if (m->maze[player->x-1][player->y] != '#' && m->maze[player->x-1][player->y] != 'X') player->x -= 1;
					else player->direction = getNewDirection(player->direction, player->name);
					break;
			}
			pthread_mutex_lock(&lock);
			resource = checkForResource(player->x, player->y);
			if (resource == 'G') {
				player->score += 10;
				std::cout << player->name << " has struck gold!" << std::endl;
				newPosition(gold);
			}
			else if (resource == 'K') {
				player->fighting = true;
				std::cout << player->name << " has entered battle!" << std::endl;
			}
			pthread_mutex_unlock(&lock);
		}
		// pthread_mutex_unlock(&lock);
		usleep(0.01 * seconds);
	}
}

int main(int argc, char const *argv[])
{
	if (signal(SIGINT,interrupted)==SIG_ERR) {
		std::cout<<"Halting."<<std::endl;
		return 1;
	}

	Player::Player* farvin = new Player::Player('1', (std::string) "Farvin");
	Player::Player* garvin = new Player::Player('2', (std::string) "Garvin");
	Player::Player* tharvin = new Player::Player('3', (std::string) "Tharvin");
	Player::Player* sally = new Player::Player('4', (std::string) "Sally");
	

	std::vector<Player::Player*> players;
	players.push_back(farvin);
	players.push_back(garvin);
	players.push_back(tharvin);
	players.push_back(sally);
	players.push_back(gold);
	players.push_back(kobold);

	m = new Maze::Maze(players);

	//farvin->ref = 'O';
	//m->printMaze();

	pthread_t ct[NUMTHREADS];

	for (int i=0;i<NUMTHREADS;i++) {
		pthread_mutex_lock(&lock);
		pthread_create(&ct[i], NULL, &run, (void *)players[i]);
		pthread_mutex_unlock(&lock);
	}

	pthread_join(ct[0], NULL);
	pthread_join(ct[1], NULL);
	pthread_join(ct[2], NULL);
	pthread_join(ct[3], NULL);

	std::cout << "---------------\n\n" << std::endl;

	pthread_mutex_lock(&lock);
	m->printMaze();
	pthread_mutex_unlock(&lock);

	while(continuing) usleep(1);
	//std::cout << "farvin x: " << farvin->x << " y: " << farvin->y << std::endl;
	for (int i = 0; i < 4; ++i)
	{
		std::cout << players[i]->name << "\tscore: " << players[i]->score << "\thealth: " << players[i]->health << std::endl;
	}
	return 0;
}