#include <iostream>
#include <random>

// MAP is FROM 0 - 1 BOTH X AND Y
// SPEED IS HOW MUCH IT MOVES PER CLICK - 0.001 means it takes a thousand clicks to go from one side of the board to the other

class Creature
{
	public:
		float x;
		float y;
		float speed;
		float energy;
		Creature(float in_x, float in_y, float in_speed, float in_energy)
		{
			x = in_x;
			y = in_y;
			speed = in_speed;
			energy = in_energy;
		}
};

int main()
{
	// Random engine for initialization
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);

    Creature creature_1(0.5, 0.5, 0.001, 1);

    std::cout << creature_1.x << std::endl;

    /*for (int n = 0; n < 100; ++n) {
       std::cout << dist(e2) << std::endl;
    }*/
}