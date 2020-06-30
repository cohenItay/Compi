#pragma once

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "table.h"
#include "Attrs.h"
#include "Parameters.h"

#define INITIAL_SIZE (1024)
#define INITIAL_FILTERED_SIZE (128)
#define GROWTH_FACTOR (2)
#define MAX_LOAD_FACTOR (1)

TableRow* internalTableRowSearch(Table* d, const char* key);
Table* internalTableCreate(int size);

/* table initialization code used in both TableCreate and grow */
Table* internalTableCreate(int size)
{
    Table* d;
    int i;

    d = calloc(1, sizeof(*d));

    assert(d != NULL);

    d->size = size;
    d->n = 0;
    return d;
}

Table* tableCreate(void)
{
    return internalTableCreate(INITIAL_SIZE);
}

Table* createChildTable(Table* parentTable) {
    Table* child = internalTableCreate(INITIAL_SIZE);
    child->parentTable = parentTable;
    parentTable->childTable = child;
    return child;
}

Table* getParentTable(Table* ofChild) {
    return ofChild->parentTable;
}

Table* tableDestroy(Table* d)
{
    int i;
    struct tableRow* e;
    struct tableRow* next;
    struct table* pTbl = d->parentTable;
    struct table* tbl = d;
    struct table* tblNext;

    while (tbl != NULL) {
        e = tbl->firstRow;
     
        while (e != NULL) {
            next = e->next;

            free(e->key);
            free(e->value);
            free(e);
            e = next;
        }
        
        tblNext = tbl->childTable;
        free(tbl);
        tbl = tblNext;
    }
    if (pTbl != NULL)
        pTbl->childTable = NULL;
    return pTbl;
}

#define MULTIPLIER (97)

static void
grow(Table* d)
{
    Table* d2;            /* new table will be created */
    struct table swap;
    int i;
    struct tableRow* e;

    e = d->firstRow;
    d2 = internalTableCreate(d->size * GROWTH_FACTOR);

    /* copy Table*/
    while (e != NULL) {
        tableInsert(d2, e->key, e->value);
        e = e->next;
    }

    swap = *d;
    *d = *d2;
    *d2 = swap;

    tableDestroy(d2);
}

/* insert a new key-value pair into an existing table */
void tableInsert(Table* d, const char* key, const AttrModel* value)
{
    struct tableRow* tr = internalTableRowSearch(d, key);

    assert(key);
    assert(value);

    if (tr == NULL) {
        tr = malloc(sizeof(struct tableRow));
        assert(tr);
        tr->key = key;
        tr->next = NULL;
        if (d->firstRow == NULL) {
            // first entry on table
            d->firstRow = tr;
            d->lastRow = tr;
        }
        else {
            d->lastRow->next = tr;
            d->lastRow = tr;
        }
    }
    tr->value = value;

    d->n++;

    /* grow table if there is not enough room */
    if (d->n >= d->size * MAX_LOAD_FACTOR) {
        grow(d);
    }
}

TableRow* internalTableRowSearch(Table* d, const char* key) {
    TableRow* tr = d->firstRow;
    if (tr == NULL) {
        return NULL;
    }

    while (tr != NULL) {
        if (!strcmp(tr->key, key)) {
            /* got it */
            return tr;
        }
        tr = tr->next;
    }

    return NULL;
}

/* return the most recently inserted value associated with a key */
/* or NULL if no matching key is present */
const AttrModel* tableSearch(Table* d, const char* key)
{
    TableRow* tr = internalTableRowSearch(d, key);
    if (tr == NULL)
        return NULL;
    else
        return tr->value;
}

/* delete the most recently inserted record with the given key */
/* if there is no such record, has no effect */
void tableDeleteRow(Table* d, const char* key)
{
    struct tableRow* prev;         
    struct tableRow* e;              /* what to delete */

    for (prev = d->firstRow; prev != NULL; prev = prev->next) {
        //check if its the first row
        if (!strcmp(prev->key, key)) {
            e = prev;
            d->firstRow = prev->next;
            free(e->value);
            free(e);
            return;

        } else if (prev->next != NULL && !strcmp(prev->next->key, key)) {
            /*got it*/
            e = prev->next;
            prev->next = e->next;
            free(e->value);
            free(e);
            return;
        }
    }
}

Table* filterByType(Table* tbl, const char* type) {
    Table* newTbl = internalTableCreate(INITIAL_FILTERED_SIZE);
    int i;
    TableRow* tr = tbl->firstRow;
    AttrModel* attrmdl;
    
    for (i = 0; i < tbl->n; i++) {
        if (!strcmp(tr->value->type, type)) {
            /* its a match */
            attrmdl = createCopyOf(tr->value);
            tableInsert(newTbl, tr->key, attrmdl);
        }
    }
    return newTbl;
}

Table* filterByRole(Table* tbl, const char* role) {
    Table* newTbl = internalTableCreate(INITIAL_FILTERED_SIZE);
    int i;
    TableRow* tr = tbl->firstRow;
    AttrModel* attrmdl;

    for (i = 0; i < tbl->n; i++) {
        if (!strcmp(tr->value->role, role)) {
            /* its a match */
            attrmdl = createCopyOf(tr->value);
            tableInsert(newTbl, tr->key, attrmdl);
        }
    }
    return newTbl;
}

void populateTableWithParameters(Table* tbl, Parameters* params) {
    AttrModel* att;
    Param* prm;
    int i;

    if (params == NULL || tbl == NULL)
        return;

    for (i = 0; i < params->amount; i++) {
        att = createAttributesModel();
        prm = params->params_array[i];
        att->name = prm->name;
        att->role = ATTR_ROLE_VAR;
        att->type = prm->type;
        att->list = prm->dimen;

        tableInsert(tbl, att->name, att);
    }
}