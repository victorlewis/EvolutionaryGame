// g++ -sim0.cpp -o sim0 && websocketd --port=8080 ./sim0

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "Eigen/Dense"

#define n 16
#define rateOfChange 0.4

using namespace Eigen;

class Creature
{
	public:
		Vector2f pos;
		float speed;
		float mass;
		float radius;
		Vector3f color;

	void recalculateRadius()
	{
		// MASS BASICALLY ACTS AS AREA
		radius = sqrt(mass/M_PI);
	}

	void moveFromVector(float xVector, float yVector)
	{
		// MULTIPLY BY THE SPEED, WHICH IS HOW MUCH A CREATURE CAN MOVE IN A "CLICK"
		float xMov = speed * xVector;
		float yMov = speed * yVector;

		pos(0) = pos(0) + xMov;
		pos(1) = pos(1) + yMov;

		// PREVENT GOING OUT OF BOUNDS
		if (pos(0)-radius < 0.0) pos(0) = 0.0+radius;
		if (pos(0)+radius > 1.0) pos(0) = 1.0-radius;
		if (pos(1)-radius < 0.0) pos(1) = 0.0+radius;
		if (pos(1)+radius > 1.0) pos(1) = 1.0-radius;

	}
};


void printCreaturesJSON(Creature* creatureArray, unsigned int numberOfCreatures)
{
	// THIS IS TEMPORARY UNTIL BINARY MESSAGES ARE IMPLENTED
	printf("[");
	for (int i = 0; i<numberOfCreatures; i++)
	{
		printf("{\"x\":%f, \"y\":%f,\"radius\":%f}", creatureArray[i].pos(0), creatureArray[i].pos(1), creatureArray[i].radius);
		if (i<n-1) printf(", ");
	}
	printf("]\n");
}

void checkCollisions(Creature* creatureArray, int numberOfCreatures)
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
					float distanceX = creatureArray[i].pos(0) - creatureArray[j].pos(0);
					float distanceY = creatureArray[i].pos(1) - creatureArray[j].pos(1);

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
						creatureArray[winnerIndex].recalculateRadius();
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

	Creature creatureArray[n];
	float vectorsX[n];
	float vectorsY[n];

	for (int i = 0; i<n; i++)
	{
		// INITIALIZE THE CREATURES WITH RANDOM POSITIONS
		creatureArray[i].pos(0) = (float)rand()/RAND_MAX;
		creatureArray[i].pos(1) = (float)rand()/RAND_MAX;
		creatureArray[i].speed = 0.005;
		creatureArray[i].mass = 0.0002 + (float)rand()/RAND_MAX * 0.0005;

		// INITIAL MOVEMENT VECTORS
		vectorsX[i] = ((float)rand()/RAND_MAX - 0.5);
		vectorsY[i] = ((float)rand()/RAND_MAX - 0.5);

		creatureArray[i].recalculateRadius();
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

			creatureArray[i].moveFromVector(vectorsX[i], vectorsY[i]);
		}

		// NOW, CHECK FOR COLLISIONS, BRUTE FORCE METHOD
		checkCollisions(creatureArray, n);

		usleep(33 * 1000); // * 1000 is to convert to miliseconds

		printCreaturesJSON(creatureArray, n);
	}
}
