#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "maze.cpp"

static const int NUMTHREADS = 4;
const float TIMESTEP_DELAY = 0.001;
const int seconds = 1000000;
volatile bool continuing = true;
volatile bool paused = false;
bool comments = false;
pthread_mutex_t lock;
Maze::Maze* m;
Player::Player* gold = new Player::Player('G', (std::string) "Gold");
Player::Player* kobold = new Player::Player('K', (std::string) "Kobold");
int pid1, pid2, status;

void sigusr1(int sig) {
	std::cout <<"SIGUSR1" << std::endl;
	m->printMaze();
	for (int i = 0; i < 4; ++i)
	{
		std::cout << m->players[i]->name << "\tscore: " << m->players[i]->score << "\thealth: " << m->players[i]->health << std::endl;
	}
}

void my_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        std::cout << "Received SIGUSR1!\n";
    }
}

void interrupted(int sig) {
	/*std::cout << std::endl;
	m->printMaze();
	for (int i = 0; i < 4; ++i)
	{
		std::cout << m->players[i]->name << "\tscore: " << m->players[i]->score << "\thealth: " << m->players[i]->health << std::endl;
	}*/
	//continuing=false;
	paused = true;
	std::cout << "Reset map? (y/n) ";
	char choice;
	std::cin >> choice;
	if (choice == 'y' || choice == 'Y') {
		m->newMaze();
	}
	paused = false;
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
		while(paused) usleep(TIMESTEP_DELAY * seconds);
		// pthread_mutex_lock(&lock);
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
					player->health -= 5;
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
				if (comments) std::cout << player->name << " has struck gold!" << std::endl;
				newPosition(gold);
			}
			else if (resource == 'K') {
				player->fighting = true;
				if (comments) std::cout << player->name << " has entered battle!" << std::endl;
			}
			pthread_mutex_unlock(&lock);
		}
		// pthread_mutex_unlock(&lock);
		usleep(TIMESTEP_DELAY * seconds);
	}
}

static void signal_handler(int signo)
{

    /* signo contains the signal number that was received */
    switch( signo )
    {
        /* Signal is a SIGUSR1 */
        case SIGUSR1:
            std::cout << "Process : received SIGUSR1 \n" << getpid();
            if(pid1==getpid()) /* it is the parent */
	    {
             std::cout << "Process  is passing SIGUSR1 to %d...\n" << getpid() << " " << pid2;
             kill( pid2, SIGUSR1 );
	    }
	    else /* it is the child */
	    {
              std::cout << "Process  is passing SIGUSR2 to itself...\n" << getpid();
              kill(getpid(), SIGUSR2);
	    }
            break;
        
        /*  It's a SIGUSR2 */
        case SIGUSR2:
            std::cout << "Process : received SIGUSR2 \n" << getpid();
            if(pid1==getpid())
	    {
             std::cout << "Process %d is passing SIGUSR2 to ...\n" << getpid() << " " << pid2 ;
             kill( pid2, SIGUSR2 );
	    }
	    else /* it is the child */
	    {
              std::cout << "Process %d will terminate itself using SIGINT\n" << getpid();
              kill(getpid(), SIGINT);
	    }
            break;

        default:
                break;
    }

    return;
}

int main(int argc, char const *argv[])
{
	//signal(SIGINT,interrupted);
	//signal(SIGUSR1, my_handler);
	if( signal( SIGUSR1, signal_handler) == SIG_ERR  )
    {
        std::cout <<"error\n";
    }

	//std::cout << SIGUSR2 << std::endl;

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