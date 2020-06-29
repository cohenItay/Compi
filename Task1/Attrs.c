#include "Attrs.h"
#include <stdlib.h>

AttrModel* createAttributesModel() {
	AttrModel* a = (AttrModel*)calloc(1, sizeof(AttrModel));
	return a;
}

AttrModel* createCopyOf(AttrModel* src) {
	AttrModel* attrmdl = createAttributesModel();
	attrmdl->list = src->list;
	attrmdl->name = src->name;
	attrmdl->role = src->role;
	attrmdl->type = src->type;
	return attrmdl;
}