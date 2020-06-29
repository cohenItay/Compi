#pragma once
#include "Attrs.h"
#include "Parameters.h"

typedef struct table Table;

typedef struct tableRow {
    struct tableRow* next;
    char* key;
    AttrModel* value;
} TableRow;

typedef struct table {
    int size;           /* size of the pointer table */
    int n;              /* number of elements stored */
    struct tableRow* firstRow;
    struct tableRow* lastRow;
    Table* parentTable;
    Table* childTable;
} Table;

/* create a new empty table */
Table* tableCreate(void);

/* destroy a table and all of his child tables. returns the parent table if exists */
Table* tableDestroy(Table*);

/* insert a new key-value pair into an existing table */
void tableInsert(Table*, const char* key, const AttrModel *value);

/* return the value associated with a given key */
/* or NULL if no matching key is present */
const AttrModel* tableSearch(Table*, const char* key);

/* create new LinkedList of TalbeRow which contains only the desired type*/
Table* filterByType(Table*, const char* type);

/* create new LinkedList of TalbeRow which contains only the desired type*/
Table* filterByRole(Table*, const char* role);

/* delete the record with the given key */
/* if there is no such record, has no effect */
void tableDeleteRow(Table*, const char* key);

Table* createChildTable(Table* parentTable);

Table* getParentTable(Table* ofChild);

/* helper function to converte Parameters to AttrModel and populate the desired table with it.*/
void populateTableWithParameters(Table* tbl, Parameters *params);