#pragma once
#include "Dimension.h"

typedef struct param {
	char* type;
	char* name;
	Dimension* dimen;
} Param;

typedef struct parameters {
	Param** params_array;
	int amount;
} Parameters;

Param* createParam();

/* validates that the two models are equal by their contents. if both are NULL it counts as equal. 0 - not equal, 1- equals*/
int paramEqual(Param*, Param*);

Parameters* createParameters();

/* validates that the two models are equal by their contents. if both are NULL it counts as equal. 0 - not equal, 1- equals*/
int parametersEqual(Parameters*, Parameters*);

/* if no such parameter already exists, add it and returns 1. otherwise return 0*/
int addToParameters(Parameters* params, Param* param);

void growParameters(Parameters* params, int growAmount);