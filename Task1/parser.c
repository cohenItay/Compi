
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


Token* currToken;

void parse() {
	parse_PROG();
	match(1, TOKEN_EOF);
}

static void parse_PROG() {
	currToken = next_token();
	switch (currToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule PROG -> GLOBAL_VARS  FUNC_PREDEFS  FUNC_FULL_DEFS\n");
			currToken = back_token();
			parse_GLOBAL_VARS();
			parse_FUNC_PREDEFS();
			parse_FUNC_FULL_DEFS();
			break;
		}
		default: {
			error_recovery(TOKEN_EOF);
		}
	}
}
 
static void parse_GLOBAL_VARS() {
	eTOKENS* self_follow;
	currToken = next_token();
	switch (currToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule GLOBAL_VARS -> VAR_DEC GLOBAL_VARS_TAG\n");
			currToken = back_token();
			parse_VAR_DEC();
			parse_GLOBAL_VARS_TAG();
			break;
		}
		default: {
			self_follow = (eTOKENS[3]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE };
			error_recovery_multi(3, self_follow);
		}
	}
}

static void parse_VAR_DEC() {
	eTOKENS* self_follow;
	currToken = next_token();
	switch (currToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule VAR_DEC -> TYPE id VAR_DEC_TAG\n");
			currToken = back_token();
			parse_TYPE();
			match(TOKEN_ID);
			parse_VAR_DEC_TAG();
			break;
		}
		default: {
			self_follow = (eTOKENS[7]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE, TOKEN_ID, TOKEN_CURLY_BRACE_OPEN, TOKEN_IF, TOKEN_RETURN };
			error_recovery_multi(7, self_follow);
		}
	}
}

static void parse_TYPE() {
	currToken = next_token();
	switch (currToken->kind) {
		case TOKEN_INTEGER_TYPE: {
			fprintf(yyout, "Rule TYPE -> int\n");
			currToken = back_token();
			match(TOKEN_INTEGER_TYPE);
			break;
		}
		case TOKEN_FLOAT_TYPE: {
			fprintf(yyout, "Rule TYPE -> float\n");
			currToken = back_token();
			match(TOKEN_FLOAT_TYPE);
			break;
		}
		default: {
			error_recovery(TOKEN_ID);
		}
	}
}

static void parse_VAR_DEC_TAG() {
	eTOKENS* self_follow;
	currToken = next_token();
	switch (currToken->kind) {
		case TOKEN_SEMICOLON: {
			fprintf(yyout, "Rule VAR_DEC_TAG -> ;\n");
			currToken = back_token();
			match(TOKEN_SEMICOLON);
			break;
		}
		case TOKEN_BRACKET_OPEN: {
			fprintf(yyout, "Rule VAR_DEC_TAG -> [ DIM_SIZES ]\n");
			currToken = back_token();
			parse_DIM_SIZES();
			match(TOKEN_BRACKET_CLOSE);
			break;
		}
		default: {
			self_follow = (eTOKENS[7]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE, TOKEN_ID, TOKEN_CURLY_BRACE_OPEN, TOKEN_IF, TOKEN_RETURN };
			error_recovery_multi(7, self_follow);
		}
	}
}

static void parse_DIM_SIZES() {
	currToken = next_token();
	switch (currToken->kind) {
		case TOKEN_INTEGER: {
			fprintf(yyout, "Rule DIM_SIZES -> int_num DIM_SIZES_TAG\n");
			currToken = back_token();
			match(TOKEN_INTEGER);
			parse_DIM_SIZES_TAG();
			break;
		}
		default: {
			error_recovery(TOKEN_BRACKET_CLOSE);
		}
	}
}

static void parse_DIM_SIZES_TAG() {
	currToken = next_token();
	switch (currToken->kind) {
		case TOKEN_COMMA: {
			fprintf(yyout, "Rule DIM_SIZES_TAG -> , DIM_SIZES\n");
			currToken = back_token();
			match(TOKEN_COMMA);
			parse_DIM_SIZES();
			break;
		}
		// DIM_SIZES_TAG supports epsilon so adding cases for Follow(DIM_SIZES_TAG):
		case TOKEN_BRACKET_CLOSE: {
			fprintf(yyout, "Rule DIM_SIZES_TAG -> epsilon\n");
			currToken = back_token();
			break;
		}
		default: {
			error_recovery(TOKEN_BRACKET_CLOSE);
		}
	}
}

static void parse_GLOBAL_VARS_TAG() {
	currToken = next_token();
}

static void parse_FUNC_PREDEFS() {
	currToken = next_token();

}

static void parse_FUNC_FULL_DEFS() {
	currToken = next_token();

}

/*
* loops threw next_token() until encountered with token supplied.
* TOKEN_EOF is being checked internally.
*/
static void error_recovery(eTOKENS t) {
	do {
		currToken = next_token();
		if (t == currToken) {
			break;
		}
	} while (currToken != TOKEN_EOF);
	currToken = back_token();
}

/*
* loops threw next_token() until encountered with one of the tokens supplied.
* TOKEN_EOF is being checked internally.
*/
static void error_recovery_multi(int count, eTOKENS* t_arr) {
	int containedInArr = 0;
	int i;
	do {
		currToken = next_token();
		for (i = 0; i < count; i++) {
			if (t_arr[i] == currToken) {
				break;
			}
		}
	} while (currToken != TOKEN_EOF);
	currToken = back_token();
}