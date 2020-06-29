#pragma once

#include "Parameters.h"
#include <stdlib.h>
#include <string.h>

Param* createParam() {
	return (Param*)calloc(1, sizeof(Param));
}

Parameters* createParameters() {
	return (Parameters*)calloc(1, sizeof(Parameters));
}

int addToParameters(Parameters* params, Param* param) {
	int index, i;
	Param* eqPrm;

	for (i = 0; i < params->amount; i++) {
		if (paramEqual(params->params_array[i], param)) {
			return 0;
		}
	}
	growParameters(params, 1);
	index = params->amount - 1;
	params->params_array[index] = param;
	return 1;
}

void growParameters(Parameters* params, int growAmount) {
	Param** temp = (Param**)calloc(params->amount + growAmount, sizeof(Param*));
	int i;
	for (i = 0; i < params->amount; i++) {
		temp[i] = params->params_array[i];
	}
	free(params->params_array);
	params->params_array = temp;
	params->amount = params->amount + growAmount;
}

int parametersEqual(Parameters* a, Parameters* b) {
	int i;
	Param* param;

	if (a == NULL && b == NULL)
		return 1;

	if (a->amount != b->amount)
		return 0;
	
	for (i = 0; i < a->amount; i++) {
		if (!paramEqual(a->params_array[i], b->params_array[i])) {
			return 0;
		}
	}
	return 1;
}

int paramEqual(Param* a, Param*b) {
	if (a == NULL && b == NULL)
		return 1;

	if (strcmp(a->type, b->type)) {
		return 0;
	} else if (!strcmp(a->name, b->name)) {
		return 1;
	}
	return dimensionEqual(a->dimen, b->dimen);
}