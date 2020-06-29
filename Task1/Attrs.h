#pragma once

#define ATTR_TYPE_INT "integer"
#define ATTR_TYPE_FLOAT "float"
#define ATTR_TYPE_ARRAY "array"
#define ATTR_TYPE_VOID "void"
#define ATTR_TYPE_ERR "error"
#define ATTR_ROLE_FULL_FUNC "full definition"
#define ATTR_ROLE_PRE_FUNC "pre-definition"
#define ATTR_ROLE_VAR "variable"

typedef struct attributes {
    char* name;
    char* role;
    char* type;
    char* arrType;
    void* list;
} AttrModel;

AttrModel* createAttributesModel();

AttrModel* createCopyOf(AttrModel* src);