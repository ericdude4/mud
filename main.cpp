#include <vector>
#include <unistd.h>
#include <pthread.h>
#include "maze.cpp"

static const int NUMTHREADS = 4;
const int TIMESTEP_DELAY = 10;
pthread_mutex_t lock;

void* run(void* my_player) {
	Player::Player* player;
	player = (Player::Player*) my_player;

	// std::cout << player->name;
	while(true) {
		std::cout << player->name;
		usleep(5000);
	}
}

int main(int argc, char const *argv[])
{
	Player::Player* farvin = new Player::Player('1', (std::string) "Farvin");
	Player::Player* garvin = new Player::Player('2', (std::string) "Garvin");
	Player::Player* tharvin = new Player::Player('3', (std::string) "Tharvin");
	Player::Player* sally = new Player::Player('4', (std::string) "Sally");
	Player::Player* gold = new Player::Player('G', (std::string) "Gold");
	Player::Player* kobold = new Player::Player('K', (std::string) "Kobold");

	std::vector<Player::Player*> players;
	players.push_back(farvin);
	players.push_back(garvin);
	players.push_back(tharvin);
	players.push_back(sally);
	players.push_back(gold);
	players.push_back(kobold);

	Maze::Maze* m = new Maze::Maze(players);

	//farvin->ref = 'O';
	//m->printMaze();

	pthread_t ct[NUMTHREADS];

	for (int i=0;i<NUMTHREADS;i++) {
		pthread_mutex_lock(&lock);
		pthread_create(&ct[i], NULL, &run, (void *)players[i]);
		pthread_mutex_unlock(&lock);
	}
	std::cout << "farvin x: " << farvin->x << " y: " << farvin->y << std::endl;

	while(true) {
		usleep(100);
	}
	//std::cout << "farvin x: " << farvin->x << " y: " << farvin->y << std::endl;

	return 0;
}