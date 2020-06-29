#pragma once

#include "Dimension.h"
#include <string.h>
#include <stdlib.h>

Dimension* createDimension() {
	return (Dimension*)calloc(1, sizeof(Dimension));
}

void addToDimension(Dimension* dimen, char dimenSize) {
	growDimension(dimen, 1);
	dimen->dimen_array[dimen->amount] = dimenSize;
}

void growDimension(Dimension* dimen, int growAmount) {
	char* temp = (char*)calloc(dimen->amount + growAmount, sizeof(char));
	int i;
	for (i = 0; i < dimen->amount; i++) {
		temp[i] = dimen->dimen_array[i];
	}
	free(dimen->dimen_array);
	dimen->dimen_array = temp;
	dimen->amount = dimen->amount+ growAmount;
}

int dimensionEqual(Dimension* a, Dimension* b) {
	int i;

	if (a == NULL && b == NULL)
		return 1;
	if (a->amount != b->amount)
		return 0;
	for (i = 0; i < a->amount; i++) {
		if (strcmp(a->dimen_array[i], b->dimen_array[i]))
			return 0;
	}
	return 1;
}