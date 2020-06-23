#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define n 16
#define rateOfChange 0.4

struct Rgb
{
	float r;
	float g;
	float b;
};

struct Creature
{
	float x;
	float y;
	float speed;
	float mass;
	float radius;
	struct Rgb color;
};

void recalculateRadius(struct Creature* inputCreature)
{
	// MASS BASICALLY ACTS AS AREA
	inputCreature->radius = sqrt(inputCreature->mass/M_PI);
}

// THIS GETS A NORMALIZED VECTOR AND MOVES THE CREATURE ACCORDING TO IT
void moveFromVector(struct Creature* inputCreature, float xVector, float yVector)
{
	// MULTIPLY BY THE SPEED, WHICH IS HOW MUCH A CREATURE CAN MOVE IN A "CLICK"
	float xMov = inputCreature->speed * xVector;
	float yMov = inputCreature->speed * yVector;

	inputCreature->x += xMov;
	inputCreature->y += yMov;

	// PREVENT GOING OUT OF BOUNDS
	if (inputCreature->x < 0.0) inputCreature->x = 0.0;
	if (inputCreature->x > 1.0) inputCreature->x = 1.0;
	if (inputCreature->y < 0.0) inputCreature->y = 0.0;
	if (inputCreature->y > 1.0) inputCreature->y = 1.0;
}

void printCreaturesJSON(struct Creature* creatureArray, unsigned int numberOfCreatures)
{
	// THIS IS TEMPORARY UNTIL BINARY MESSAGES ARE IMPLENTED
	printf("[");
	for (int i = 0; i<numberOfCreatures; i++)
	{
		printf("{\"x\":%f, \"y\":%f,\"radius\":%f}", creatureArray[i].x, creatureArray[i].y, creatureArray[i].radius);
		if (i<n-1) printf(", ");
	}
	printf("]\n");
}

void checkCollisions(struct Creature* creatureArray, int numberOfCreatures)
{
	for (int i = 0; i<numberOfCreatures; i++)
	{
		if (creatureArray[i].mass > 0)
		{
			for (int j = 0; j<numberOfCreatures; j++)
			{
				if (i != j && creatureArray[j].mass > 0) // IF NOT SELF
				{
					// GET THE DISTANCE BETWEEN THE TWO CREATURES
					float distanceX = creatureArray[i].x - creatureArray[j].x;
					float distanceY = creatureArray[i].y - creatureArray[j].y;

					float distance = sqrt(distanceX*distanceX+distanceY*distanceY);

					if (distance - creatureArray[i].radius - creatureArray[j].radius <= 0.0) // COLLISION!
					{
						// GET THE BIGGEST CREATURE
						int winnerIndex = i;
						int loserIndex = j;

						if (creatureArray[i].mass < creatureArray[j].mass)
						{
							winnerIndex = j;
							loserIndex = i;
						}

						creatureArray[winnerIndex].mass += creatureArray[loserIndex].mass;
						creatureArray[loserIndex].mass = 0.0;
						recalculateRadius(&creatureArray[winnerIndex]);
						creatureArray[loserIndex].radius = 0.0;
					}

				}
			}				
		}
	}
}

int main()
{
    setbuf(stdout, NULL); // Disable output buffering, essential for websocketd

	srand(time(NULL)); // RNG INI

	struct Creature creatureArray[n];
	float vectorsX[n];
	float vectorsY[n];

	for (int i = 0; i<n; i++)
	{
		// INITIALIZE THE CREATURES WITH RANDOM POSITIONS
		creatureArray[i].x = (float)rand()/RAND_MAX;;
		creatureArray[i].y = (float)rand()/RAND_MAX;;
		creatureArray[i].speed = 0.005;
		creatureArray[i].mass = 0.0002 + (float)rand()/RAND_MAX * 0.0005;

		// INITIAL MOVEMENT VECTORS
		vectorsX[i] = ((float)rand()/RAND_MAX - 0.5);
		vectorsY[i] = ((float)rand()/RAND_MAX - 0.5);

		recalculateRadius(&creatureArray[i]);
	}

	printCreaturesJSON(creatureArray, n);
	// MOVE THE CREATURES
	while (1)
	{
		// MAKE THE CREATURES MOVE
		for (int i = 0; i<n; i++)
		{
			vectorsX[i] += ((float)rand()/RAND_MAX - 0.5) *rateOfChange;
			vectorsY[i] += ((float)rand()/RAND_MAX - 0.5) *rateOfChange;

			if (vectorsX[i] > 0.777) vectorsX[i] = 0.777;
			if (vectorsX[i] < -0.777) vectorsX[i] = -0.777;
			if (vectorsY[i] > 0.777) vectorsY[i] = 0.777;
			if (vectorsY[i] < -0.777) vectorsY[i] = -0.777;

			moveFromVector(&creatureArray[i], vectorsX[i], vectorsY[i]);
		}

		// NOW, CHECK FOR COLLISIONS, BRUTE FORCE METHOD
		checkCollisions(creatureArray, n);

		usleep(33 *1000); // * 1000 is to convert to miliseconds

		printCreaturesJSON(creatureArray, n);
	}
}
