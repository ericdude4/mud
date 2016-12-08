#include <vector>
#include "maze.cpp"

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

	farvin->ref = 'O';
	m->printMaze();
	return 0;
}