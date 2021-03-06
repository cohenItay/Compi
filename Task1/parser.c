
#include "parser.h"
#include "Token.h"
#include "Table.h"
#include "Dimension.h"
#include "Parameters.h"
#include "Attrs.h"

static void error_recovery_multi(int count, eTOKENS* t_arr);
static void error_recovery(eTOKENS t);
static void parse_PROG();
static void parse_GLOBAL_VARS();
static AttrModel* parse_VAR_DEC();
static Dimension* parse_VAR_DEC_TAG();
static char* parse_TYPE();
static void parse_DIM_SIZES(Dimension* dim);
static void parse_DIM_SIZES_TAG(Dimension* dim);
static void parse_GLOBAL_VARS_TAG();
static void parse_FUNC_PREDEFS();
static void parse_FUNC_FULL_DEFS();
static AttrModel* parse_FUNC_PROTOTYPE();
static char* parse_RETURNED_TYPE();
static Parameters* parse_PARAMS();
static Param* parse_PARAM();
static void parse_PARAM_TAG(Param*);
static void parse_PARAM_TAG_TAG(Param*);
static void parse_PARAM_LIST(Parameters*);
static void parse_PARAM_LIST_TAG(Parameters*);
static void parse_FUNC_PREDEFS_TAG();
static void parse_FUNC_WITH_BODY();
static void parse_FUNC_FULL_DEFS_TAG();
static void parse_COMP_STMT(AttrModel*);
static void parse_VAR_DEC_LIST();
static void parse_VAR_DEC_LIST_TAG();
static void parse_STMT_LIST(AttrModel*);
static void parse_STMT_LIST_TAG(AttrModel*);
static void parse_STMT(AttrModel*);
static void parse_STMT_TAG(AttrModel*);
static void parse_IF_STMT(AttrModel*);
static void parse_ARGS(AttrModel*);
static void parse_ARG_LIST(AttrModel*);
static void parse_ARG_LIST_TAG(AttrModel*, Parameters*, int);
static void parse_RETURN_STMT(AttrModel*);
static void parse_RETURN_STMT_TAG(AttrModel*);
static void parse_VAR_TAG(AttrModel*);
static void parse_EXPR_LIST(AttrModel*);
static void parse_EXPR_LIST_TAG(AttrModel*, int);
static void parse_CONDITION();
static AttrModel* parse_EXPR();
static void parse_EXPR_TAG(AttrModel*);
static AttrModel* parse_TERM();
static void parse_TERM_TAG(AttrModel*);
static AttrModel* parse_FACTOR();
static void parse_FACTOR_TAG(AttrModel*);

static void validateFunctionsImplementations();

Token* nextToken;
Table* mTbl;

void parse() {
	setParserTokenPtr(&nextToken);
	mTbl = tableCreate();
	parse_PROG();
	match(TOKEN_EOF);
	tableDestroy(mTbl);
}

static void parse_PROG() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule PROG -> GLOBAL_VARS  FUNC_PREDEFS  FUNC_FULL_DEFS\n");
			parse_GLOBAL_VARS();
			parse_FUNC_PREDEFS();
			parse_FUNC_FULL_DEFS();
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE); // for printing the error.
			error_recovery(TOKEN_EOF);
		}
	}
}

static void parse_GLOBAL_VARS() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule GLOBAL_VARS -> VAR_DEC GLOBAL_VARS_TAG\n");
			parse_VAR_DEC();
			parse_GLOBAL_VARS_TAG();
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			self_follow = (eTOKENS[3]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE };
			error_recovery_multi(3, self_follow);
		}
	}
}

static AttrModel* parse_VAR_DEC() {
	AttrModel* attrModel = createAttributesModel();
	eTOKENS* self_follow;
	
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule VAR_DEC -> TYPE id VAR_DEC_TAG\n");
			attrModel->type = parse_TYPE();
			attrModel->value = (nextToken->kind == TOKEN_INTEGER_TYPE) ? "0" : NULL; // not supporting float values
			match(TOKEN_ID);
			attrModel->name = nextToken->lexeme; // match has updated nextToken to point on the id
			attrModel->list = parse_VAR_DEC_TAG();
			attrModel->role = ATTR_ROLE_VAR;
			tableInsert(mTbl, attrModel->name, attrModel);
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			self_follow = (eTOKENS[7]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE, TOKEN_ID, TOKEN_CURLY_BRACE_OPEN, TOKEN_IF, TOKEN_RETURN };
			error_recovery_multi(7, self_follow);
		}
	}
}

static char* parse_TYPE() {
	char* attrType = NULL;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE: {
			attrType = ATTR_TYPE_INT;
			fprintf(yyout, "Rule TYPE -> int\n");
			match(TOKEN_INTEGER_TYPE);
			break;
		}
		case TOKEN_FLOAT_TYPE: {
			attrType = ATTR_TYPE_FLOAT;
			fprintf(yyout, "Rule TYPE -> float\n");
			match(TOKEN_FLOAT_TYPE);
			break;

		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			error_recovery(TOKEN_ID);
		}
	}
	return attrType;
}

static Dimension* parse_VAR_DEC_TAG() {
	Dimension* dimen = createDimension();
	eTOKENS* self_follow;

	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_SEMICOLON: {
			fprintf(yyout, "Rule VAR_DEC_TAG -> ;\n");
			match(TOKEN_SEMICOLON);
			break;
		}
		case TOKEN_BRACKET_OPEN: {
			fprintf(yyout, "Rule VAR_DEC_TAG -> [ DIM_SIZES ]\n");
			match(TOKEN_BRACKET_OPEN);
			parse_DIM_SIZES(dimen);
			match(TOKEN_BRACKET_CLOSE);
			match(TOKEN_SEMICOLON);
			break;
		}
		default: {
			match_multi(2, TOKEN_SEMICOLON, TOKEN_BRACKET_OPEN);
			self_follow = (eTOKENS[7]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE, TOKEN_ID, TOKEN_CURLY_BRACE_OPEN, TOKEN_IF, TOKEN_RETURN };
			error_recovery_multi(7, self_follow);
		}
	}
	return dimen;
}

static void parse_DIM_SIZES(Dimension* dim) {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER: {
			fprintf(yyout, "Rule DIM_SIZES -> int_num DIM_SIZES_TAG\n");
			match(TOKEN_INTEGER);
			addToDimension(dim, nextToken->lexeme);
			parse_DIM_SIZES_TAG(dim);
			break;
		}
		default: {
			match(TOKEN_INTEGER);
			error_recovery(TOKEN_BRACKET_CLOSE);
		}
	}
}

static void parse_DIM_SIZES_TAG(Dimension* dim) {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_COMMA: {
			fprintf(yyout, "Rule DIM_SIZES_TAG -> , DIM_SIZES\n");
			match(TOKEN_COMMA);
			parse_DIM_SIZES(dim);
			break;
		}
		// DIM_SIZES_TAG supports epsilon so adding cases for Follow(DIM_SIZES_TAG):
		case TOKEN_BRACKET_CLOSE: {
			fprintf(yyout, "Rule DIM_SIZES_TAG -> epsilon\n");
			break;
		}
		default: {
			match_multi(2, TOKEN_COMMA, TOKEN_BRACKET_CLOSE);
			error_recovery(TOKEN_BRACKET_CLOSE);
		}
	}
}

static void parse_GLOBAL_VARS_TAG() {
	// GLOBAL_VARS_TAG supports epsilon so adding cases for Follow(GLOBAL_VARS_TAG):
	// PROBLEM: some values for First(GLOBAL_VARS_TAG) and Follow(GLOBAL_VARS_TAG) are the same. (TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE)
	// SOLVED BY: checks 3 steps ahead, if its TOKEN_SEMICOLON OR TOKEN_BRACKET_OPEN- we will treat the mutual (TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE) as First(_GLOBAL_VARS_TAG)
	// otherwise, if its TOKEN_PARENTHESE_OPEN we will treat the them as Follow(_GLOBAL_VARS_TAG). if no such of the 3 ahead exists this is an error.
	eTOKENS* self_follow;
	eTOKENS threeAheadKind;
	nextToken = next_token();
	next_token();
	threeAheadKind = next_token()->kind;
	back_token();
	back_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			if (threeAheadKind == TOKEN_SEMICOLON || threeAheadKind == TOKEN_BRACKET_OPEN) {
				//Treat as First(GLOBAL_VARS_TAG)
				fprintf(yyout, "Rule GLOBAL_VARS_TAG-> VAR_DEC GLOBAL_VARS_TAG\n");
				parse_VAR_DEC();
				parse_GLOBAL_VARS_TAG();
				break;
			}
			else if (threeAheadKind == TOKEN_PARENTHESE_OPEN) {
				//Treat as Follow(GLOBAL_VARS_TAG)
				fprintf(yyout, "Rule GLOBAL_VARS_TAG-> epsilon\n");
				break;
			}
		}
		// only case left to be added as a part of Follow(GOLBAL_VARS_TAG)
		case TOKEN_VOID_TYPE: {
			fprintf(yyout, "Rule GLOBAL_VARS_TAG-> epsilon\n");
			break;
		}
		default: {
			match_multi(3, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE);
			self_follow = (eTOKENS[3]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE };
			error_recovery_multi(3, self_follow);
		}
	}
}

static void parse_FUNC_PREDEFS() {
	AttrModel* attrModel;
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE:
		case TOKEN_VOID_TYPE: {
			fprintf(yyout, "Rule FUNC_PREDEFS -> FUNC_PROTOTYPE; FUNC_PREDEFS_TAG\n");
			attrModel = parse_FUNC_PROTOTYPE();
			attrModel->role = ATTR_ROLE_PRE_FUNC;
			tableInsert(mTbl, attrModel->name, attrModel);
			match(TOKEN_SEMICOLON);
			parse_FUNC_PREDEFS_TAG();
			break;
		}
		default: {
			match_multi(3, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE);
			self_follow = (eTOKENS[3]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE };
			error_recovery_multi(3, self_follow);
		}
	}
}

static AttrModel* parse_FUNC_PROTOTYPE() {
	AttrModel* attrModel = createAttributesModel();
	eTOKENS* self_follow;
	
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE:
		case TOKEN_VOID_TYPE: {
			fprintf(yyout, "Rule FUNC_PROTOTYPE->RETURNED_TYPE id(PARAMS)\n");
			attrModel->type = parse_RETURNED_TYPE();
			match(TOKEN_ID);
			attrModel->name = nextToken->lexeme;
			match(TOKEN_PARENTHESE_OPEN);
			attrModel->list = parse_PARAMS();
			match(TOKEN_PARENTHESE_CLOSE);
			break;
		}
		default: {
			match_multi(3, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE);
			self_follow = (eTOKENS[2]){ TOKEN_CURLY_BRACE_OPEN, TOKEN_SEMICOLON };
			error_recovery_multi(2, self_follow);
		}
	}
	return attrModel;
}

static char* parse_RETURNED_TYPE() {
	const char* type = NULL;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule RETURNED_TYPE -> TYPE\n");
			type = parse_TYPE();
			break;
		}
		case TOKEN_VOID_TYPE: {
			fprintf(yyout, "Rule RETURNED_TYPE -> void\n");
			match(TOKEN_VOID_TYPE);
			type = ATTR_TYPE_VOID;
			break;
		}
		default: {
			match_multi(3, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE);
			error_recovery(TOKEN_ID);
		}
	}
	return type;
}

static Parameters* parse_PARAMS() {
	Parameters* params = NULL;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule PARAMS -> PARAM_LIST\n");
			params = createParameters();
			parse_PARAM_LIST(params);
			break;
		}
		// PARAMS supports epsilon so adding cases for Follow(PARAMS):
		case TOKEN_PARENTHESE_CLOSE: {
			fprintf(yyout, "Rule PARAMS -> epsilon\n");
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
	return params;
}

static void parse_PARAM_LIST(Parameters* params) {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule PARAM_LIST -> PARAM PARAM_LIST_TAG\n");
			addToParameters(params, parse_PARAM());
			parse_PARAM_LIST_TAG(params);
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}

static Param* parse_PARAM() {
	Param* param = NULL;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule PARAM -> TYPE PARAM_TAG\n");
			param = createParam();
			param->type = parse_TYPE();
			parse_PARAM_TAG(param);
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
	return param;
}

static void parse_PARAM_TAG(Param* param) {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_ID: {
			fprintf(yyout, "Rule PARAM_TAG -> id PARAM_TAG_TAG\n");
			param->name = nextToken->lexeme;
			match(TOKEN_ID);
			parse_PARAM_TAG_TAG(param);
			break;
		}
		default: {
			match(TOKEN_ID);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}

static void parse_PARAM_TAG_TAG(Param* param) {
	Dimension* dimen;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_BRACKET_OPEN: {
			fprintf(yyout, "Rule PARAM_TAG_TAG -> [ DIM_SIZES ]\n");
			match(TOKEN_BRACKET_OPEN);
			dimen = createDimension();
			param->dimen = dimen;
			parse_DIM_SIZES(param->dimen);
			match(TOKEN_BRACKET_CLOSE);
			break;
		}
		 // PARAM_TAG_TAG supports epsilon so adding cases for Follow(PARAM_TAG_TAG):
		case TOKEN_COMMA:
		case TOKEN_PARENTHESE_CLOSE: {
			fprintf(yyout, "Rule PARAM_TAG_TAG -> epsilon\n");
			break;
		}
		default: {
			match(TOKEN_ID);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}

static void parse_PARAM_LIST_TAG(Parameters* params) {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_COMMA: {
			fprintf(yyout, "Rule PARAM_LIST_TAG -> , PARAM PARAM_LIST_TAG\n");
			match(TOKEN_COMMA);
			if (!addToParameters(params, parse_PARAM())) {
				fprintf(yyout, "CONTEXT EXCEPTION: duplicate parameters. line %d\n", nextToken->lineNumber);
			}
			parse_PARAM_LIST_TAG(params);
			break;
		}
		// PARAM_LIST_TAG supports epsilon so adding cases for Follow(PARAM_LIST_TAG):
		case TOKEN_PARENTHESE_CLOSE: {
			fprintf(yyout, "Rule PARAM_LIST_TAG -> epsilon\n");
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}

static void parse_FUNC_PREDEFS_TAG() {
	// FUNC_PREDEFS_TAG supports epsilon so adding cases for Follow(FUNC_PREDEFS_TAG):
	// PROBLEM: all of the values for First(FUNC_PREDEFS_TAG) and Follow(FUNC_PREDEFS_TAG) are the same.
	// SOLVED BY: loops next_token() until find '{' OR ';' OR 'EOF', if its '{'  - i'll use epsilon. if its ';' i'll continue to parse.

	eTOKENS* self_follow;
	AttrModel* attrModel, * temp;
	int stepsTaken = 0;
	eTOKENS judger = TOKEN_NULL;
	while (judger != TOKEN_CURLY_BRACE_OPEN && judger != TOKEN_SEMICOLON && judger != TOKEN_EOF) {
		stepsTaken++;
		judger = next_token()->kind;
	}
	while (stepsTaken > 0) {
		back_token();
		stepsTaken--;
	}

	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE:
		case TOKEN_VOID_TYPE: {
			if (judger == TOKEN_SEMICOLON) {
				fprintf(yyout, "Rule  FUNC_PREDEFS_TAG -> FUNC_PROTOTYPE; FUNC_PREDEFS_TAG\n");
				attrModel = parse_FUNC_PROTOTYPE();
				temp = tableSearch(mTbl, attrModel->name);
				attrModel->role = ATTR_ROLE_PRE_FUNC;
				if (temp != NULL &&
					!strcmp(temp->role, ATTR_ROLE_PRE_FUNC) &&
					((Parameters*)temp->list)->amount == ((Parameters*)attrModel->list)->amount)
				{
					fprintf(yyout, "CONTEXT ERROR: Function of the same name has already been define in this scope. line %d\n", nextToken->lineNumber);
				}
				tableInsert(mTbl, attrModel->name, attrModel);
				match(TOKEN_SEMICOLON);
				parse_FUNC_PREDEFS_TAG();
				break;
			}
			else if (judger == TOKEN_CURLY_BRACE_OPEN) {
				fprintf(yyout, "Rule FUNC_PREDEFS_TAG -> epsilon\n");
				break;
			}
			// somewhere in the future tokens unexpected EOF arrived, continue to default
		}
		default: {
			match_multi(3, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE);
			self_follow = (eTOKENS[3]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE };
			error_recovery_multi(3, self_follow);
		}
	}

}

static void parse_FUNC_FULL_DEFS() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE:
		case TOKEN_VOID_TYPE: {
			fprintf(yyout, "Rule FUNC_FULL_DEFS -> FUNC_WITH_BODY FUNC_FULL_DEFS_TAG\n");
			parse_FUNC_WITH_BODY();
			parse_FUNC_FULL_DEFS_TAG();
			break;
		}
		default: {
			match_multi(3, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE);
			self_follow = (eTOKENS[4]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE, TOKEN_EOF };
			error_recovery_multi(4, self_follow);
		}
	}
}

static void parse_FUNC_WITH_BODY() {
	eTOKENS* self_follow;
	AttrModel *attrParseModel, *attrTableModel;
	nextToken = next_token();

	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE:
		case TOKEN_VOID_TYPE: {
			fprintf(yyout, "Rule FUNC_WITH_BODY  -> FUNC_PROTOTYPE COMP_STMT\n");
			attrParseModel = parse_FUNC_PROTOTYPE();
			if (strcmp(attrParseModel->name, "main")) {
				// its not the main function

				attrTableModel = tableSearch(mTbl, attrParseModel->name);
				if (attrTableModel == NULL) {
					fprintf(yyout, "CONTEXT ERROR: no decleration for function %s. line %d\n", attrParseModel->name, nextToken->lineNumber);
				}
				else {
					if (strcmp(attrTableModel->role, ATTR_ROLE_PRE_FUNC)) {
						// not equals.
						fprintf(yyout, "CONTEXT ERROR: no decleration for function %s. line %d\n", attrParseModel->name, nextToken->lineNumber);
					}
					if (strcmp(attrTableModel->type, attrParseModel->type)) {
						fprintf(yyout, "CONTEXT ERROR: function %s implementation return type differs from decleration. line %d\n", attrParseModel->name, nextToken->lineNumber);
					}
					if (!parametersEqual(attrTableModel->list, attrParseModel->list)) {
						fprintf(yyout, "CONTEXT ERROR: function %s implementation parameters differ from what is declared. line %d\n", attrParseModel->name, nextToken->lineNumber);
					}
					if (!strcmp(attrTableModel->role, ATTR_ROLE_PRE_FUNC) &&
						!strcmp(attrTableModel->type, attrParseModel->type) &&
						parametersEqual(attrTableModel->list, attrParseModel->list)) {
						attrParseModel->role = ATTR_ROLE_FULL_FUNC;
						tableInsert(mTbl, attrParseModel->name, attrParseModel);
					}
				}
			}
			mTbl = createChildTable(mTbl);
			populateTableWithParameters(mTbl, ((Parameters*)attrParseModel->list));
			parse_COMP_STMT(attrParseModel);
			mTbl = tableDestroy(mTbl); // returns the parent table after destruction
			break;
		}
		default: {
			match_multi(3, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE);
			self_follow = (eTOKENS[4]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE, TOKEN_EOF };
			error_recovery_multi(4, self_follow);
		}
	}
}

static void parse_FUNC_FULL_DEFS_TAG() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_VOID_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule FUNC_FULL_DEFS_TAG -> FUNC_WITH_BODY\n");
			parse_FUNC_FULL_DEFS();
			break;
		}
		// nullable rule, add special case from Follow(FUNC_FULL_DEFS_TAG):
		case TOKEN_EOF: {
			fprintf(yyout, "Rule FUNC_FULL_DEFS_TAG -> epsilon\n");
			validateFunctionsImplementations();
			break;
		}
		default: {
			match_multi(3, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE);
			self_follow = (eTOKENS[4]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE, TOKEN_EOF };
			error_recovery_multi(4, self_follow);
		}
	}
}

/* validates that all of the pre defined function are implemented. if not prints error*/
static void validateFunctionsImplementations() {
	int i;
	TableRow *row;
	Table *predefsTbl;

	predefsTbl = filterByRole(mTbl, ATTR_ROLE_PRE_FUNC);
	if (predefsTbl->n == 0) {
		return;
	} else {
		// missing function implementation
		row = predefsTbl->firstRow;

		for (i = 0; i < predefsTbl->n; i++) {
			fprintf(yyout, "CONTEXT ERROR: no implementation for function %s. line %d\n", row->key, nextToken->lineNumber);
			row = row->next;
		}
	}
}

/* inheritedAttrs reflect the the function prototype which this body belongs to*/
void parse_COMP_STMT(AttrModel* inheritedAttrs) {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_CURLY_BRACE_OPEN: {
			fprintf(yyout, "Rule COMP_STMT -> { VAR_DEC_LIST STMT_LIST }\n");
			match(TOKEN_CURLY_BRACE_OPEN);
			parse_VAR_DEC_LIST();
			parse_STMT_LIST(inheritedAttrs);
			match(TOKEN_CURLY_BRACE_CLOSE);
			break;
		}
		default: {
			match(TOKEN_CURLY_BRACE_OPEN);
			self_follow = (eTOKENS[2]){ TOKEN_CURLY_BRACE_CLOSE, TOKEN_SEMICOLON };
			error_recovery_multi(2, self_follow);
		}
	}
}

/* responsible for variable declerations inside a function body */
static void parse_VAR_DEC_LIST() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule VAR_DEC_LIST -> VAR_DEC_LIST_TAG\n");
			parse_VAR_DEC_LIST_TAG();
			break;
		}
		// VAR_DEC_LIST supports epsilon so adding cases for Follow(VAR_DEC_LIST):
		case TOKEN_ID:
		case TOKEN_CURLY_BRACE_OPEN:
		case TOKEN_IF:
		case TOKEN_RETURN: {
			fprintf(yyout, "Rule VAR_DEC_LIST -> epsilon\n");
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			self_follow = (eTOKENS[4]){ TOKEN_ID, TOKEN_CURLY_BRACE_OPEN, TOKEN_IF, TOKEN_RETURN };
			error_recovery_multi(4, self_follow);
		}
	}
}
static void parse_VAR_DEC_LIST_TAG() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule VAR_DEC_LIST_TAG -> VAR_DEC VAR_DEC_LIST_TAG\n");
			parse_VAR_DEC();
			parse_VAR_DEC_LIST_TAG();
			break;
		}
		// VAR_DEC_LIST_TAG supports epsilon so adding cases for Follow(VAR_DEC_LIST_TAG):
		case TOKEN_ID:
		case TOKEN_CURLY_BRACE_OPEN:
		case TOKEN_IF:
		case TOKEN_RETURN: {
			fprintf(yyout, "Rule VAR_DEC_LIST_TAG -> epsilon\n");
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			self_follow = (eTOKENS[4]){ TOKEN_ID, TOKEN_CURLY_BRACE_OPEN, TOKEN_IF, TOKEN_RETURN };
			error_recovery_multi(4, self_follow);
		}
	}
}

/* inheritedAttrs reflect the the function prototype which this body belongs to*/
static void parse_STMT_LIST(AttrModel* inheritedAttr) {
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_CURLY_BRACE_OPEN:
		case TOKEN_IF:
		case TOKEN_RETURN: {
			fprintf(yyout, "Rule STMT_LIST ->  STMT STMT_LIST_TAG\n");
			parse_STMT(inheritedAttr);
			parse_STMT_LIST_TAG(inheritedAttr);
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_CURLY_BRACE_OPEN, TOKEN_IF, TOKEN_RETURN);
			error_recovery(TOKEN_CURLY_BRACE_CLOSE);
		}
	}
}

/* inheritedAttrs reflect the the function prototype which this body belongs to*/
static void parse_STMT_LIST_TAG(AttrModel* inhertiedModel) {
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_SEMICOLON: {
			fprintf(yyout, "Rule STMT_LIST_TAG -> ; STMT STMT_LIST_TAG\n");
			match(TOKEN_SEMICOLON);
			parse_STMT(inhertiedModel);
			parse_STMT_LIST_TAG(inhertiedModel);
			break;
		}
		// rule supports epsilon so adding cases for Follow(STMT_LIST_TAG):
		case TOKEN_CURLY_BRACE_CLOSE: {
			fprintf(yyout, "Rule STMT_LIST_TAG -> epsilon\n");
			break;
		}
		default: {
			match(TOKEN_SEMICOLON); 
			error_recovery(TOKEN_CURLY_BRACE_CLOSE);
		}
	}
}

/* inheritedAttrs reflect the the function prototype which this body belongs to*/
static void parse_STMT(AttrModel* inheritedAttr) {
	eTOKENS* self_follow;
	AttrModel* idAttrs;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID: {
			// excpection a declared variable / function usage.
			fprintf(yyout, "Rule STMT ->  id STMT_TAG\n");
			idAttrs = tableSearch(mTbl, nextToken->lexeme); // might belong to a variable or a function
			match(TOKEN_ID);
			parse_STMT_TAG(idAttrs);
			break;
		}
		case TOKEN_CURLY_BRACE_OPEN: {
			fprintf(yyout, "Rule STMT ->  COMP_STMT\n");
			mTbl = createChildTable(mTbl);
			parse_COMP_STMT(inheritedAttr);
			mTbl = tableDestroy(mTbl); // returns the parent table
			break;
		}
		case TOKEN_IF: {
			fprintf(yyout, "Rule STMT ->  IF_STMT\n");
			parse_IF_STMT(inheritedAttr);
			break;
		}
		case TOKEN_RETURN: {
			fprintf(yyout, "Rule STMT ->  RETURN_STMT\n");
			parse_RETURN_STMT(inheritedAttr);
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_CURLY_BRACE_OPEN, TOKEN_IF, TOKEN_RETURN); 
			self_follow = (eTOKENS[2]){ TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE };
			error_recovery_multi(2, self_follow);
		}
	}
}

/* idAttrs reflect the the function / variable which was previously declared and now required to be used*/
static void parse_STMT_TAG(AttrModel* idAttrs) {
	eTOKENS* self_follow;
	AttrModel* funAttrs;
	nextToken = next_token();
	back_token();
	AttrModel *varAttrs, *exprAttrs;

	switch (nextToken->kind) {
		case TOKEN_AR_EQUAL:
		case TOKEN_BRACKET_OPEN: {
			// this is a variable, might be array.
			fprintf(yyout, "Rule STMT_TAG ->    VAR# = EXPR \n");
			parse_VAR_TAG(idAttrs);
			match(TOKEN_AR_EQUAL);
			exprAttrs = parse_EXPR();
			if (!strcmp(idAttrs->type, ATTR_TYPE_INT) && strcmp(exprAttrs->type, ATTR_TYPE_INT)) {
				fprintf(yyout, "CONTEXT ERROR: Illegal assignment. line %d\n", nextToken->lineNumber);
			}
			if (!strcmp(idAttrs->type, ATTR_TYPE_FLOAT) &&
				(strcmp(exprAttrs->type, ATTR_TYPE_INT) || strcmp(exprAttrs->type, ATTR_TYPE_FLOAT))) {
				fprintf(yyout, "CONTEXT ERROR: Illegal assignment. line %d\n", nextToken->lineNumber);
			}
			break;
		}
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule STMT_TAG ->   ( ARGS )\n");
			nextToken = back_token(); // for retreiving the desired function name 
			funAttrs = tableSearch(mTbl, nextToken->lexeme);
			if (funAttrs != NULL &&
				strcmp(funAttrs->role, ATTR_ROLE_PRE_FUNC) || strcmp(funAttrs->role, ATTR_ROLE_FULL_FUNC)) 
			{
				fprintf(yyout, "%s is not a function. line %d\n", nextToken->lineNumber, nextToken->lineNumber);
				idAttrs->type = ATTR_TYPE_ERR;
			}
			nextToken = next_token(); // restore token location
			match(TOKEN_PARENTHESE_OPEN);
			parse_ARGS(funAttrs);
			match(TOKEN_PARENTHESE_CLOSE);
			break;
		}
		default: {
			match_multi(2, TOKEN_ID, TOKEN_PARENTHESE_OPEN);
			self_follow = (eTOKENS[2]){ TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE };
			error_recovery_multi(2, self_follow);
		}
	}
}

/* inheritedAttrs reflect the the function prototype which this body belongs to*/
static void parse_IF_STMT(AttrModel* inheritedAttrs) {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_IF: {
			fprintf(yyout, "Rule IF_STMT ->  if (CONDITION) STMT\n");
			match(TOKEN_IF);
			match(TOKEN_PARENTHESE_OPEN);
			parse_CONDITION();
			match(TOKEN_PARENTHESE_CLOSE);
			parse_STMT(inheritedAttrs);
			break;
		}
		default: {
			match_multi(1, TOKEN_IF);
			self_follow = (eTOKENS[2]){ TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE };
			error_recovery_multi(2, self_follow);
		}
	}
}

static void parse_ARGS(AttrModel* funcAttrs) {
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule _ARGS ->  ARG_LIST \n");
			parse_ARG_LIST(funcAttrs);
			break;
		}
		// rule supports epsilon so adding cases for Follow(X):
		case TOKEN_PARENTHESE_CLOSE: {
			fprintf(yyout, "Rule _ARGS -> epsilon\n");
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN); 
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}

static void parse_ARG_LIST(AttrModel* funcAttrs) {
	Parameters* params;
	AttrModel* exprAttrs;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule ARG_LIST ->  EXPR ARG_LIST_TAG\n");
			exprAttrs = parse_EXPR();
			params = (Parameters*)funcAttrs->list;
			if (params->amount > 0 &&
				strcmp(exprAttrs->type, params->params_array[0]->type)) {
				fprintf("CONTEXT ERROR: type mismatch for first argument for function, line %d\n", nextToken->lineNumber);
			}
			parse_ARG_LIST_TAG(funcAttrs, params, 1);
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}
static void parse_ARG_LIST_TAG(AttrModel* inheritedAttrs, Parameters* params, int checkedParamIndex) {
	AttrModel* exprAttrs;
	Param* checkedParam = params->amount > checkedParamIndex ? params->params_array[checkedParamIndex] : NULL;
	nextToken = next_token();
	back_token(); 
	switch (nextToken->kind) {
		case TOKEN_COMMA: {
			fprintf(yyout, "Rule ARG_LIST_TAG ->   , EXPR ARG_LIST_TAG\n");
			if (checkedParam == NULL) {
				fprintf(yyout, "CONTEXT EXCEPTION: Illegal amount of parameters. line %d\n", nextToken->lineNumber);
				inheritedAttrs->type = ATTR_TYPE_ERR;
			}
			match(TOKEN_COMMA);
			exprAttrs = parse_EXPR();
			if (strcmp(exprAttrs->type, checkedParam->type)) {
				fprintf(yyout, "CONTEXT EXCEPTION: mismatch parameter type at line %d\n", nextToken->lineNumber);
				inheritedAttrs->type = ATTR_TYPE_ERR;
			}
			parse_ARG_LIST_TAG(inheritedAttrs, params, checkedParamIndex+1);
			break;
		}
		// rule supports epsilon so adding cases for Follow(ARG_LIST_TAG):
		case TOKEN_PARENTHESE_CLOSE: {
			fprintf(yyout, "Rule ARG_LIST_TAG -> epsilon\n");
			if (checkedParam != NULL) {
				fprintf(yyout, "CONTEXT EXCEPTION: Illegal amount of parameters. line %d\n", nextToken->lineNumber);
				inheritedAttrs->type == ATTR_TYPE_ERR;
			}
			break;
		}
		default: {
			match_multi(1, TOKEN_COMMA);
			error_recovery(TOKEN_CURLY_BRACE_CLOSE);
		}
	}
}
static void parse_RETURN_STMT(AttrModel* inheritedAttrs) {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_RETURN: {
			fprintf(yyout, "Rule RETURN_STMT ->   return RETURN_STMT_TAG\n");
			match(TOKEN_RETURN);
			parse_RETURN_STMT_TAG(inheritedAttrs);
			break;
		}
		default: {
			match_multi(1, TOKEN_RETURN);
			self_follow = (eTOKENS[2]){ TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE };
			error_recovery_multi(2, self_follow);
		}
	}
}
static void parse_RETURN_STMT_TAG(AttrModel* inheritedAttrs) {
	eTOKENS* self_follow;
	AttrModel* exprAttrs;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule RETURN_STMT_TAG ->  EXPR \n");
			exprAttrs = parse_EXPR();
			if (strcmp(exprAttrs->type, inheritedAttrs->type)) {
				fprintf(yyout, "CONTEXT ERROR: Excpected return type of %s but %s found. line %d\n", inheritedAttrs->type, exprAttrs->type, nextToken->lineNumber);
			}
			break;
		}
		// rule supports epsilon so adding cases for Follow(RETURN_STMT_TAG):
		case TOKEN_SEMICOLON:
		case TOKEN_CURLY_BRACE_CLOSE: {
			fprintf(yyout, "Rule RETURN_STMT_TAG -> epsilon\n");
			if (strcmp(ATTR_TYPE_VOID, inheritedAttrs->type)) {
				fprintf(yyout, "CONTEXT ERROR: Excpected return type of void but %s found. line %d\n", inheritedAttrs->type, nextToken->lineNumber);
			}
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			self_follow = (eTOKENS[2]){ TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE };
			error_recovery_multi(2, self_follow);
		}
	}
}

/* idAttrs reflect the the variable which was previously declared and now required to be used. might be array*/
static void parse_VAR_TAG(AttrModel* varAttrs) {
	eTOKENS* self_follow;

	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_BRACKET_OPEN: {
			// should be an array
			fprintf(yyout, "Rule VAR_TAG -> [ EXPR_LIST ]\n");
			match(TOKEN_BRACKET_OPEN);
			if (varAttrs->list == NULL || ((Dimension*)varAttrs->list)->amount < 1) {
				fprintf(yyout, "variable %s should be an array or multi dimensional. line %d\n", varAttrs->name, nextToken->lineNumber);
				varAttrs->type = ATTR_TYPE_ERR;
			}
			parse_EXPR_LIST(varAttrs);
			match(TOKEN_BRACKET_CLOSE);
			break;
		}
		// rule supports epsilon so adding cases for Follow(VAR_TAG):
		case TOKEN_AR_MUL:
		case TOKEN_AR_ADD:
		case TOKEN_SEMICOLON:
		case TOKEN_CURLY_BRACE_CLOSE:
		case TOKEN_COMMA:
		case TOKEN_BRACKET_CLOSE:
		case TOKEN_COMP_E:
		case TOKEN_COMP_NE:
		case TOKEN_COMP_GT:
		case TOKEN_COMP_GTE:
		case TOKEN_COMP_LT:
		case TOKEN_COMP_LTE:
		case TOKEN_PARENTHESE_CLOSE:
		case TOKEN_AR_EQUAL: {
			fprintf(yyout, "Rule VAR_TAG -> epsilon\n");
			if (varAttrs->list != NULL && ((Dimension*)varAttrs->list)->amount > 0) {
				fprintf(yyout, "CONEXT ERROR: variable %s cannot be an array or have a dimension\n", varAttrs->name);
				varAttrs->type = ATTR_TYPE_ERR;
			}
			break;
		}
		default: {
			match_multi(1, TOKEN_BRACKET_OPEN);
			self_follow = (eTOKENS[14]){ TOKEN_AR_MUL, TOKEN_AR_ADD, TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE, TOKEN_COMMA, TOKEN_BRACKET_CLOSE,
				TOKEN_COMP_E, TOKEN_COMP_NE, TOKEN_COMP_GT, TOKEN_COMP_GTE, TOKEN_COMP_LT, TOKEN_COMP_LTE, TOKEN_PARENTHESE_CLOSE };
			error_recovery_multi(14, self_follow);
		}
	}
}

/* this function parses the indexes when array is used.  var attrs - the attrs of the id being used*/ 
static void parse_EXPR_LIST(AttrModel* varAttrs) {
	nextToken = next_token();
	back_token();
	AttrModel* exprAttrs;
	Dimension* varDimen;

	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule EXPR_LIST ->  EXPR EXPR_LIST_TAG\n");
			exprAttrs = parse_EXPR();
			varDimen = (Dimension*)varAttrs->list;
			if (strcmp(exprAttrs->type, ATTR_TYPE_INT)) {
				fprintf(yyout, "CONTEXT EXCEPTION: the expression must return an integer. line %d\n", nextToken->lineNumber);
				varAttrs->type = ATTR_TYPE_ERR;
			} else if (varDimen->amount >0 &&
				atoi(exprAttrs->value) >= atoi(varDimen->dimen_array[0])) 
			{
				fprintf(yyout, "CONTEXT ERROR: the expression %s exceeded the array size. line %d\n", exprAttrs->name, nextToken->lineNumber);
				varAttrs->type = ATTR_TYPE_ERR;
			}
			parse_EXPR_LIST_TAG(varAttrs, 1);
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			error_recovery(TOKEN_BRACKET_CLOSE);
		}
	}
}
static void parse_EXPR_LIST_TAG(AttrModel* varAttrs, int indexToCheck) {
	AttrModel* exprAttrs;
	Dimension* varDimens = (Dimension*)varAttrs->list;
	char* checkedDimen = varDimens->amount > indexToCheck ? varDimens->dimen_array[indexToCheck] : NULL;
	if (checkedDimen == NULL) {
		fprintf(yyout, "CONTEXT ERROR: too many dimensions supplied for array %s. line %d\n", varAttrs->name, nextToken->lineNumber);
	}
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_COMMA: {
			fprintf(yyout, "Rule EXPR_LIST_TAG ->   , EXPR EXPR_LIST_TAG\n");
			match(TOKEN_COMMA);
			exprAttrs = parse_EXPR();
			if (strcmp(exprAttrs->type, ATTR_TYPE_INT)) {
				fprintf(yyout, "CONTEXT EXCEPTION: the expression must return an integer. line %d\n", nextToken->lineNumber);
				varAttrs->type = ATTR_TYPE_ERR;
			} else if (varDimens->amount > 0 &&
				atoi(exprAttrs->value) >= atoi(checkedDimen))
			{
				fprintf(yyout, "CONTEXT ERROR: the expression %s exceeded the array size. line %d\n", exprAttrs->name, nextToken->lineNumber);
				varAttrs->type = ATTR_TYPE_ERR;
			}
			parse_EXPR_LIST_TAG(varAttrs, indexToCheck+1);
			break;
		}
		// rule supports epsilon so adding cases for Follow(X):
		case TOKEN_BRACKET_CLOSE: {
			fprintf(yyout, "Rule EXPR_LIST_TAG -> epsilon\n");
			if (checkedDimen != NULL) {
				fprintf(yyout, "CONTEXT ERROR: illegal amount of dimensions. line %d\n", nextToken->lineNumber);
			}
			break;
		}
		default: {
			match(TOKEN_COMMA);
			error_recovery(TOKEN_BRACKET_CLOSE);
		}
	}
}

static void parse_CONDITION() {
	nextToken = next_token();
	back_token();
	AttrModel* exprAttrs;

	switch (nextToken->kind) {
		case TOKEN_ID: 
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule CONDITION -> EXPR rel_op EXPR\n");
			exprAttrs =  parse_EXPR();
			if (exprAttrs->type == ATTR_TYPE_ERR) {
				fprintf(yyout, "CONTEXT ERROR: type not supported for expression %s. line %d\n", exprAttrs->name, nextToken->lineNumber);
			}
			match_multi(6,TOKEN_COMP_E,TOKEN_COMP_NE,TOKEN_COMP_GT,TOKEN_COMP_GTE,TOKEN_COMP_LT,TOKEN_COMP_LTE);
			exprAttrs = parse_EXPR();
			if (exprAttrs->type == ATTR_TYPE_ERR) {
				fprintf(yyout, "CONTEXT ERROR: type not supported for expression %s. line %d\n", exprAttrs->name, nextToken->lineNumber);
			}
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}

/* reflects an addition of terms one to another. 
( basically reflects integer / float / array / function or abbrevation of those types)*/
static AttrModel* parse_EXPR() {
	eTOKENS* self_follow;
	AttrModel* attrs = NULL;

	nextToken = next_token();
	back_token();	
	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule EXPR ->  TERM EXPR_TAG\n");
			attrs = parse_TERM();
			parse_EXPR_TAG(attrs);
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			self_follow = (eTOKENS[11]){ TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE, TOKEN_COMMA, TOKEN_BRACKET_CLOSE, TOKEN_COMP_E,
				TOKEN_COMP_NE, TOKEN_COMP_GT, TOKEN_COMP_GTE, TOKEN_COMP_LT, TOKEN_COMP_LTE, TOKEN_PARENTHESE_CLOSE };
			error_recovery_multi(11, self_follow);
		}
	}
	return attrs;
}

static void parse_EXPR_TAG(AttrModel* exprAttrs) {
	eTOKENS* self_follow;
	AttrModel* termAttrs;
	int result;
	int length;
	char* buff; // string in heap
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_AR_ADD: {
			fprintf(yyout, "Rule EXPR_TAG ->  + TERM EXPR_TAG\n");
			match(TOKEN_AR_ADD);
			termAttrs = parse_TERM();
			if ( !strcmp(termAttrs->type, ATTR_TYPE_ERR) || !strcmp(exprAttrs->type, ATTR_TYPE_ERR)) {
				exprAttrs->type = ATTR_TYPE_ERR;
			} else if (!strcmp(termAttrs->type, ATTR_TYPE_INT) && !strcmp(exprAttrs->type, ATTR_TYPE_INT)) {
				result = atoi(exprAttrs->value) + atoi(termAttrs->value);
				length = snprintf(NULL, 0, "%d", result); // digits length
				buff = malloc(sizeof(char)*(length + 1)); // allocate enough in heap
				snprintf(buff, length + 1, "%d", result); // converte result to string
				exprAttrs->value = buff;
			} else {
				// th new term type is float. change the outcome type:
				exprAttrs->type = ATTR_TYPE_FLOAT;
				exprAttrs->value = NULL; // TODO: SHOULD BE THE ADDITION BETWEEN FLOAT AND INT
			}
			free(termAttrs);
			parse_EXPR_TAG(exprAttrs);
			break;
		}
		// rule supports epsilon so adding cases for Follow(EXPR_TAG):
		case TOKEN_SEMICOLON :
		case TOKEN_CURLY_BRACE_CLOSE:
		case TOKEN_PARENTHESE_CLOSE:
		case TOKEN_COMMA:
		case TOKEN_BRACKET_CLOSE:
		case TOKEN_COMP_E:
		case TOKEN_COMP_NE:
		case TOKEN_COMP_GT:
		case TOKEN_COMP_GTE:
		case TOKEN_COMP_LT:
		case TOKEN_COMP_LTE: {
			fprintf(yyout, "Rule EXPR_TAG ->  epsilon\n");
			break;
		}
		default: {
			match(TOKEN_AR_ADD);
			self_follow = (eTOKENS[11]){ TOKEN_SEMICOLON,TOKEN_CURLY_BRACE_CLOSE,TOKEN_PARENTHESE_CLOSE,TOKEN_COMMA,TOKEN_BRACKET_CLOSE,
				TOKEN_COMP_E, TOKEN_COMP_NE,TOKEN_COMP_GT,TOKEN_COMP_GTE,	TOKEN_COMP_LT,TOKEN_COMP_LTE };
			error_recovery_multi(11, self_follow);
		}
	}
}

/* reflects an multiplextion of factors one to another.
( basically reflects integer / float / array / function or abbrevation of those types)*/
static AttrModel* parse_TERM() {
	eTOKENS* self_follow;
	AttrModel* fctrAttrs = NULL;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule TERM ->  FACTOR TERM_TAG\n");
			fctrAttrs = parse_FACTOR();
			parse_TERM_TAG(fctrAttrs);
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			self_follow = (eTOKENS[12]){ TOKEN_AR_ADD, TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE, TOKEN_COMMA, TOKEN_BRACKET_CLOSE,
				TOKEN_COMP_E, TOKEN_COMP_NE, TOKEN_COMP_GT, TOKEN_COMP_GTE, TOKEN_COMP_LT, TOKEN_COMP_LTE, TOKEN_PARENTHESE_CLOSE };
			error_recovery_multi(12, self_follow);
		}
	}
	return fctrAttrs;
}

static void parse_TERM_TAG(AttrModel* fctrAttrs) {
	eTOKENS* self_follow;
	AttrModel* newFctrAttrs;
	int result;
	int length;
	char* buff; // string in heap
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_AR_MUL: {
			fprintf(yyout, "Rule TERM_TAG ->  * FACTOR TERM_TAG\n");
			match(TOKEN_AR_MUL);
			newFctrAttrs = parse_FACTOR();
			if (!strcmp(fctrAttrs->type, ATTR_TYPE_ERR) || !strcmp(newFctrAttrs->type, ATTR_TYPE_ERR)) {
				fctrAttrs->type = ATTR_TYPE_ERR;
				fctrAttrs->value = NULL;
			} else if (!strcmp(fctrAttrs->type, ATTR_TYPE_INT) || !strcmp(newFctrAttrs->type, ATTR_TYPE_INT)) {
				fctrAttrs->type = ATTR_TYPE_INT;
				result = atoi(fctrAttrs->value) * atoi(newFctrAttrs->value);
				length = snprintf(NULL, 0, "%d", result); // digits length
				buff = malloc(sizeof(char) * (length + 1)); // allocate enough in heap
				snprintf(buff, length + 1, "%d", result); // converte result to string
				fctrAttrs->value = result;
			} else {
				fctrAttrs->type = ATTR_TYPE_FLOAT;
				fctrAttrs->value = NULL; // TODO: should calculated with int and float addition..
			}
			free(newFctrAttrs);
			parse_TERM_TAG(fctrAttrs);
			break;
		}
		// rule supports epsilon so adding cases for Follow(TERM_TAG):
		case TOKEN_AR_ADD:
		case TOKEN_SEMICOLON:
		case TOKEN_CURLY_BRACE_CLOSE:
		case TOKEN_COMMA:
		case TOKEN_BRACKET_CLOSE:
		case TOKEN_COMP_E:
		case TOKEN_COMP_NE:
		case TOKEN_COMP_GT:
		case TOKEN_COMP_GTE:
		case TOKEN_COMP_LT:
		case TOKEN_COMP_LTE:
		case TOKEN_PARENTHESE_CLOSE: {
			fprintf(yyout, "Rule TERM_TAG -> epsilon\n");
			break;
		}
		default: {
			match(TOKEN_AR_MUL);
			self_follow = (eTOKENS[12]){ TOKEN_AR_ADD, TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE, TOKEN_COMMA, TOKEN_BRACKET_CLOSE,
				TOKEN_COMP_E, TOKEN_COMP_NE, TOKEN_COMP_GT, TOKEN_COMP_GTE, TOKEN_COMP_LT, TOKEN_COMP_LTE, TOKEN_PARENTHESE_CLOSE };
			error_recovery_multi(12, self_follow);
		}
	}
}
static AttrModel* parse_FACTOR() {
	eTOKENS* self_follow;
	AttrModel* attrs = createAttributesModel();
	AttrModel* exprAttrs;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID: {
			fprintf(yyout, "Rule FACTOR ->  id FACTOR_TAG\n");
			attrs->name = nextToken->lexeme;
			match(TOKEN_ID);
			parse_FACTOR_TAG(attrs->name);
			break;
		}
		case TOKEN_INTEGER: {
			fprintf(yyout, "Rule FACTOR ->  int_num \n");
			match(TOKEN_INTEGER);
			attrs->type = ATTR_TYPE_INT;
			attrs->value = nextToken->lexeme;
			break;
		}
		case TOKEN_FLOAT: {
			fprintf(yyout, "Rule FACTOR ->  float_num  \n");
			attrs->type = ATTR_TYPE_FLOAT;
			attrs->value = nextToken->lexeme;
			match(TOKEN_FLOAT);
			break;
		}
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule FACTOR ->  (EXPR)\n");
			match(TOKEN_PARENTHESE_OPEN);
			exprAttrs = parse_EXPR();
			attrs->type = exprAttrs->type;
			attrs->value = exprAttrs->value;
			free(exprAttrs);
			match(TOKEN_PARENTHESE_CLOSE);
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			self_follow = (eTOKENS[13]){ TOKEN_AR_MUL, TOKEN_AR_ADD, TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE, TOKEN_COMMA, TOKEN_BRACKET_CLOSE,
				TOKEN_COMP_E, TOKEN_COMP_NE, TOKEN_COMP_GT, TOKEN_COMP_GTE, TOKEN_COMP_LT, TOKEN_COMP_LTE, TOKEN_PARENTHESE_CLOSE };
			error_recovery_multi(13, self_follow);
		}
	}
	return attrs;
}

/* 
idAttrs reflect the the function / variable which was previously
declared and now required to be used
*/
static void parse_FACTOR_TAG(char* idName) {
	eTOKENS* self_follow;
	AttrModel* idAttrs = tableSearch(mTbl, idName);
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_BRACKET_OPEN: {
			fprintf(yyout, "Rule FACTOR_TAG ->  VAR_TAG\n");
			parse_VAR_TAG(idAttrs);
			if (idAttrs != NULL) {
				if (strcmp(idAttrs->role, ATTR_ROLE_VAR)) {
					fprintf(yyout, "CONTEXT ERROR: %s is not a variable. line %d\n", idAttrs->name, nextToken->lineNumber);
				}
				idAttrs->value = strcmp(idAttrs->type, ATTR_TYPE_INT) ? NULL : "0";
			}
			break;
		}
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule FACTOR_TAG ->  ( ARGS )\n");
			match(TOKEN_PARENTHESE_OPEN);
			parse_ARGS(idAttrs);
			if (idAttrs != NULL) {
				if (strcmp(idAttrs->role, ATTR_ROLE_PRE_FUNC) || strcmp(idAttrs->role, ATTR_ROLE_FULL_FUNC)) {
					fprintf(yyout, "CONTEXT ERROR: %s is not a function. line %d\n", idAttrs->name, nextToken->lineNumber);
				}
				idAttrs->value = strcmp(idAttrs->type, ATTR_TYPE_INT) ? NULL : "0";
			}
			match(TOKEN_PARENTHESE_CLOSE);
			break;
		}
		// rule supports epsilon so adding cases for Follow(FACTOR_TAG):
		case TOKEN_AR_MUL:
		case TOKEN_AR_ADD:
		case TOKEN_SEMICOLON:
		case TOKEN_CURLY_BRACE_CLOSE:
		case TOKEN_COMMA:
		case TOKEN_BRACKET_CLOSE:
		case TOKEN_COMP_E:
		case TOKEN_COMP_NE:
		case TOKEN_COMP_GT:
		case TOKEN_COMP_GTE:
		case TOKEN_COMP_LT:
		case TOKEN_COMP_LTE:
		case TOKEN_PARENTHESE_CLOSE: {
			fprintf(yyout, "Rule FACTOR_TAG -> epsilon\n");
			break;
		}
		default: {
			match_multi(2, TOKEN_BRACKET_OPEN, TOKEN_PARENTHESE_OPEN);
			self_follow = (eTOKENS[13]){ TOKEN_AR_MUL, TOKEN_AR_ADD, TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE, TOKEN_COMMA, TOKEN_BRACKET_CLOSE,
				TOKEN_COMP_E, TOKEN_COMP_NE, TOKEN_COMP_GT, TOKEN_COMP_GTE, TOKEN_COMP_LT, TOKEN_COMP_LTE, TOKEN_PARENTHESE_CLOSE };
			error_recovery_multi(13, self_follow);
		}
	}
}





/*
* check if the current token equls to the token supplied, otherwise
* loops threw next_token() until encountered with token supplied.
* TOKEN_EOF is being checked internally.
*/
static void error_recovery(eTOKENS follow) {
	do {
		if (follow == nextToken) {
			break;
		}
		nextToken = next_token();
	} while (nextToken->kind != TOKEN_EOF);
	nextToken = back_token();
}
/*
* check if one of the tokens supplied equals to current token, otherwise
* loops threw next_token() until encountered with one of the tokens supplied.
* TOKEN_EOF is being checked internally.
*/
static void error_recovery_multi(int count, eTOKENS* follows) {
	int containedInArr = 0;
	int i;
	do {
		for (i = 0; i < count; i++) {
			if (follows[i] == nextToken) {
				break;
			}
		}
		nextToken = next_token();
	} while (nextToken->kind != TOKEN_EOF);
	nextToken = back_token();
}
