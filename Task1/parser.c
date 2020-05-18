
#include "parser.h"
#include "Token.h"

static void error_recovery_multi(int count, eTOKENS* t_arr);
static void error_recovery(eTOKENS t);
static void parse_PROG();
static void parse_GLOBAL_VARS();
static void parse_VAR_DEC();
static void parse_VAR_DEC_TAG();
static void parse_TYPE();
static void parse_DIM_SIZES();
static void parse_DIM_SIZES_TAG();
static void parse_GLOBAL_VARS_TAG();
static void parse_FUNC_PREDEFS();
static void parse_FUNC_FULL_DEFS();
static void parse_FUNC_PROTOTYPE();
static void parse_RETURNED_TYPE();
static void parse_PARAMS();
static void parse_PARAM();
static void parse_PARAM_TAG();
static void parse_PARAM_TAG_TAG();
static void parse_PARAM_LIST();
static void parse_PARAM_LIST_TAG();
static void parse_FUNC_PREDEFS_TAG();


Token* nextToken;

void parse() {
	parse_PROG();
	match(TOKEN_EOF);
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

static void parse_VAR_DEC() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule VAR_DEC -> TYPE id VAR_DEC_TAG\n");
			parse_TYPE();
			match(TOKEN_ID);
			parse_VAR_DEC_TAG();
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			self_follow = (eTOKENS[7]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE, TOKEN_ID, TOKEN_CURLY_BRACE_OPEN, TOKEN_IF, TOKEN_RETURN };
			error_recovery_multi(7, self_follow);
		}
	}
}

static void parse_TYPE() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE: {
			fprintf(yyout, "Rule TYPE -> int\n");
			match(TOKEN_INTEGER_TYPE);
			break;
		}
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule TYPE -> float\n");
			match(TOKEN_FLOAT_TYPE);
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			error_recovery(TOKEN_ID);
		}
	}
}

static void parse_VAR_DEC_TAG() {
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
			fprintf(yyout, "Rule VAR_DEC_TAG -> [ DIM_SIZES ];\n");
			parse_DIM_SIZES();
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
}

static void parse_DIM_SIZES() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER: {
			fprintf(yyout, "Rule DIM_SIZES -> int_num DIM_SIZES_TAG\n");
			match(TOKEN_INTEGER);
			parse_DIM_SIZES_TAG();
			break;
		}
		default: {
			match(TOKEN_INTEGER);
			error_recovery(TOKEN_BRACKET_CLOSE);
		}
	}
}

static void parse_DIM_SIZES_TAG() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_COMMA: {
			fprintf(yyout, "Rule DIM_SIZES_TAG -> , DIM_SIZES\n");
			match(TOKEN_COMMA);
			parse_DIM_SIZES();
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
			} else if (threeAheadKind == TOKEN_PARENTHESE_OPEN) {
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
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE:
		case TOKEN_VOID_TYPE: {
			fprintf(yyout, "Rule FUNC_PREDEFS -> FUNC_PROTOTYPE; FUNC_PREDEFS_TAG\n");
			parse_FUNC_PROTOTYPE();
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

static void parse_FUNC_PROTOTYPE() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE:
		case TOKEN_VOID_TYPE: {
			fprintf(yyout, "Rule FUNC_PROTOTYPE->RETURNED_TYPE id(PARAMS)\n");
			parse_RETURNED_TYPE();
			match(TOKEN_ID);
			match(TOKEN_PARENTHESE_OPEN);
			parse_PARAMS();
			match(TOKEN_PARENTHESE_CLOSE);
			break;
		}
		default: {
			match_multi(3, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE);
			self_follow = (eTOKENS[2]){ TOKEN_CURLY_BRACE_OPEN, TOKEN_SEMICOLON };
			error_recovery_multi(2, self_follow);
		}
	}
}

static void parse_RETURNED_TYPE() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule RETURNED_TYPE -> TYPE\n");
			parse_TYPE();
			break;
		}
		case TOKEN_VOID_TYPE: {
			fprintf(yyout, "Rule RETURNED_TYPE -> void\n");
			match(TOKEN_VOID_TYPE);
			break;
		}
		default: {
			match_multi(3, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE);
			error_recovery(TOKEN_ID);
		}
	}
}

static void parse_PARAMS() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule PARAMS -> PARAM_LIST\n");
			parse_PARAM_LIST();
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
}

static void parse_PARAM_LIST() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule PARAM_LIST -> PARAM PARAM_LIST_TAG\n");
			parse_PARAM();
			parse_PARAM_LIST_TAG();
			break;
		}	
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}

static void parse_PARAM() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule PARAM -> TYPE PARAM_TAG\n");
			parse_TYPE();
			parse_PARAM_TAG();
			break;
		}
		default: {
			match_multi(2, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}

static void parse_PARAM_TAG() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_ID: {
			fprintf(yyout, "Rule PARAM_TAG -> id PARAM_TAG_TAG\n");
			match(TOKEN_ID);
			parse_PARAM_TAG_TAG();
			break;
		}
		default: {
			match(TOKEN_ID);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}

static void parse_PARAM_TAG_TAG() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_BRACKET_OPEN: {
			fprintf(yyout, "Rule PARAM_TAG_TAG -> [ DIM_SIZES ]\n");
			match(TOKEN_BRACKET_OPEN);
			parse_DIM_SIZES();
			match(TOKEN_BRACKET_CLOSE);
			break;
		}
		// PARAM_TAG_TAG supports epsilon so adding cases for Follow(PARAM_TAG_TAG):
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

static void parse_PARAM_LIST_TAG() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_COMMA: {
			fprintf(yyout, "Rule PARAM_LIST_TAG -> , PARAM PARAM_LIST_TAG\n");
			match(TOKEN_COMMA);
			parse_PARAM();
			parse_PARAM_LIST_TAG();
			break;
		}
		// PARAM_TAG_TAG supports epsilon so adding cases for Follow(PARAM_TAG_TAG):
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
	nextToken = next_token();
}

static void parse_FUNC_FULL_DEFS() {
	nextToken = next_token();
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
