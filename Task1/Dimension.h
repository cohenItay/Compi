#pragma once
typedef struct dimension {
	char* dimen_array;
	int amount;
} Dimension;

Dimension* createDimension();

/* validates that the two models are equal by their contents. if both are NULL it counts as equal. 0 - not equal, 1- equals*/
int dimensionEqual(Dimension*, Dimension*);

void addToDimension(Dimension* dimen, char dimenSize);

void growDimension(Dimension* dimen, int growAmount);
