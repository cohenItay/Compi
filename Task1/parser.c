
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
static void parse_FUNC_WITH_BODY();
static void parse_FUNC_FULL_DEFS_TAG();
static void parse_COMP_STMT();
static void parse_VAR_DEC_LIST();
static void parse_VAR_DEC_LIST_TAG();
static void parse_STMT_LIST();
static void parse_STMT_LIST_TAG();
static void parse_STMT();
static void parse_STMT_TAG();
static void parse_IF_STMT();
static void parse_ARGS();
static void parse_ARG_LIST();
static void parse_ARG_LIST_TAG();
static void parse_RETURN_STMT();
static void parse_RETURN_STMT_TAG();
static void parse_VAR_TAG();
static void parse_EXPR_LIST();
static void parse_EXPR_LIST_TAG();
static void parse_CONDITION();
static void parse_EXPR();
static void parse_EXPR_TAG();
static void parse_TERM();
static void parse_TERM_TAG();
static void parse_FACTOR();
static void parse_FACTOR_TAG();


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
			fprintf(yyout, "Rule VAR_DEC_TAG -> [ DIM_SIZES ]\n");
			parse_DIM_SIZES();
			match(TOKEN_BRACKET_CLOSE);
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
	// FUNC_PREDEFS_TAG supports epsilon so adding cases for Follow(FUNC_PREDEFS_TAG):
	// PROBLEM: all of the values for First(FUNC_PREDEFS_TAG) and Follow(FUNC_PREDEFS_TAG) are the same.
	// SOLVED BY: loops next_token() until find '{' OR ';' OR 'EOF', if its '{'  - i'll use epsilon. if its ';' i'll continue to parse.

	eTOKENS* self_follow;
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
				parse_FUNC_PROTOTYPE();
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
			match(TOKEN_CURLY_BRACE_OPEN);
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
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE:
		case TOKEN_VOID_TYPE: {
			fprintf(yyout, "Rule FUNC_WITH_BODY  -> FUNC_PROTOTYPE COMP_STMT\n");
			parse_FUNC_PROTOTYPE();
			parse_COMP_STMT();
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
	eTOKENS judger;
	nextToken = next_token();
	judger = next_token()->kind;
	back_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_INTEGER_TYPE:
		case TOKEN_FLOAT_TYPE: {
			//special logic for nullable rule which has same values for First(FUNC_FULL_DEFS_TAG) and Follow(FUNC_FULL_DEFS_TAG):
			if (judger == TOKEN_INTEGER_TYPE || judger == TOKEN_FLOAT_TYPE) {
				fprintf(yyout, "Rule FUNC_FULL_DEFS_TAG -> FUNC_WITH_BODY\n");
				parse_FUNC_WITH_BODY();
				break;
			}
			else if (judger == TOKEN_EOF) {
				fprintf(yyout, "Rule FUNC_FULL_DEFS_TAG -> epsilon\n");
				break;
			}
			// judger should be TOKEN EOF OR TOKEN_INTEGER_TYPE OR TOKEN_FLOAT_TYPE, continue to default 
		}
		default: {
			match_multi(3, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE);
			self_follow = (eTOKENS[4]){ TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE, TOKEN_VOID_TYPE, TOKEN_EOF };
			error_recovery_multi(4, self_follow);
		}
	}
}


static void parse_COMP_STMT() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_CURLY_BRACE_OPEN: {
			fprintf(yyout, "Rule COMP_STMT -> { VAR_DEC_LIST STMT_LIST }\n");
			match(TOKEN_CURLY_BRACE_OPEN);
			parse_VAR_DEC_LIST();
			parse_STMT_LIST();
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
static void parse_STMT_LIST() {
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_CURLY_BRACE_OPEN:
		case TOKEN_IF:
		case TOKEN_RETURN: {
			fprintf(yyout, "Rule STMT_LIST ->  STMT STMT_LIST_TAG\n");
			parse_STMT();
			parse_STMT_LIST_TAG();
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_CURLY_BRACE_OPEN, TOKEN_IF, TOKEN_RETURN);
			error_recovery(TOKEN_CURLY_BRACE_CLOSE);
		}
	}
}
static void parse_STMT_LIST_TAG() {
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_SEMICOLON: {
			fprintf(yyout, "Rule STMT_LIST_TAG -> ; STMT STMT_LIST_TAG\n");
			match(TOKEN_SEMICOLON);
			parse_STMT();
			parse_STMT_LIST_TAG();
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
static void parse_STMT() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID: {
			fprintf(yyout, "Rule STMT ->  id STMT_TAG\n");
			match(TOKEN_ID);
			parse_STMT_TAG();
			break;
		}
		case TOKEN_CURLY_BRACE_OPEN: {
			fprintf(yyout, "Rule STMT ->  COMP_STMT\n");
			parse_COMP_STMT();
			break;
		}
		case TOKEN_IF: {
			fprintf(yyout, "Rule STMT ->  IF_STMT\n");
			parse_IF_STMT();
			break;
		}
		case TOKEN_RETURN: {
			fprintf(yyout, "Rule STMT ->  RETURN_STMT\n");
			parse_RETURN_STMT();
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_CURLY_BRACE_OPEN, TOKEN_IF, TOKEN_RETURN); 
			self_follow = (eTOKENS[2]){ TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE };
			error_recovery_multi(2, self_follow);
		}
	}
}
static void parse_STMT_TAG() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID: {
			fprintf(yyout, "Rule STMT_TAG ->    VAR# =EXPR \n");
			parse_VAR_TAG();
			match(TOKEN_AR_EQUAL);
			parse_EXPR();
			break;
		}
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule STMT_TAG ->   ( ARGS )\n");
			match(TOKEN_PARENTHESE_OPEN);
			parse_ARGS();
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
static void parse_IF_STMT() {
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
			parse_STMT();
			break;
		}
		default: {
			match_multi(1, TOKEN_IF);
			self_follow = (eTOKENS[2]){ TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE };
			error_recovery_multi(2, self_follow);
		}
	}
}

static void parse_ARGS() {
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule _ARGS ->  ARG_LIST \n");
			parse_ARG_LIST();
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
static void parse_ARG_LIST() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule ARG_LIST ->  EXPR ARG_LIST_TAG\n");
			parse_EXPR();
			parse_ARG_LIST_TAG();
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}
static void parse_ARG_LIST_TAG() {
	nextToken = next_token();
	back_token();
	switch (nextToken->kind) {
		case TOKEN_COMMA: {
			fprintf(yyout, "Rule ARG_LIST_TAG ->   , EXPR ARG_LIST_TAG\n");
			match(TOKEN_COMMA);
			parse_ARG_LIST_TAG();
			break;
		}
		// rule supports epsilon so adding cases for Follow(ARG_LIST_TAG):
		case TOKEN_PARENTHESE_CLOSE: {
			fprintf(yyout, "Rule ARG_LIST_TAG -> epsilon\n");
			break;
		}
		default: {
			match_multi(1, TOKEN_COMMA);
			error_recovery(TOKEN_CURLY_BRACE_CLOSE);
		}
	}
}
static void parse_RETURN_STMT() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_RETURN: {
			fprintf(yyout, "Rule RETURN_STMT ->   return RETURN_STMT_TAG\n");
			match(TOKEN_RETURN);
			parse_RETURN_STMT_TAG();
			break;
		}
		default: {
			match_multi(1, TOKEN_RETURN);
			self_follow = (eTOKENS[2]){ TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE };
			error_recovery_multi(2, self_follow);
		}
	}
}
static void parse_RETURN_STMT_TAG() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule RETURN_STMT_TAG ->  EXPR \n");
			parse_EXPR();
			break;
		}
		// rule supports epsilon so adding cases for Follow(RETURN_STMT_TAG):
		case TOKEN_SEMICOLON:
		case TOKEN_CURLY_BRACE_CLOSE: {
			fprintf(yyout, "Rule RETURN_STMT_TAG -> epsilon\n");
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			self_follow = (eTOKENS[2]){ TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE };
			error_recovery_multi(2, self_follow);
		}
	}
}

static void parse_VAR_TAG() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_BRACKET_OPEN: {
			fprintf(yyout, "Rule VAR_TAG -> [ EXPR_LIST ]\n");
			match(TOKEN_BRACKET_OPEN);
			parse_EXPR_LIST();
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
static void parse_EXPR_LIST() {
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule EXPR_LIST ->  EXPR EXPR_LIST_TAG\n");
			parse_EXPR();
			parse_EXPR_LIST_TAG();
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			error_recovery(TOKEN_BRACKET_CLOSE);
		}
	}
}
static void parse_EXPR_LIST_TAG() {
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_COMMA: {
			fprintf(yyout, "Rule EXPR_LIST_TAG ->   , EXPR EXPR_LIST_TAG\n");
			match(TOKEN_COMMA);
			parse_EXPR();
			parse_EXPR_LIST_TAG();
			break;
		}
		// rule supports epsilon so adding cases for Follow(X):
		case TOKEN_BRACKET_CLOSE: {
			fprintf(yyout, "Rule EXPR_LIST_TAG -> epsilon\n");
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

	switch (nextToken->kind) {
		case TOKEN_ID: 
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule CONDITION -> EXPR rel_op EXPR\n");
			parse_EXPR();
			match_multi(6,TOKEN_COMP_E,TOKEN_COMP_NE,TOKEN_COMP_GT,TOKEN_COMP_GTE,TOKEN_COMP_LT,TOKEN_COMP_LTE);
			parse_EXPR();
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			error_recovery(TOKEN_PARENTHESE_CLOSE);
		}
	}
}

static void parse_EXPR() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule EXPR ->  TERM EXPR_TAG\n");
			parse_TERM();
			parse_EXPR_TAG();
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			self_follow = (eTOKENS[11]){ TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE, TOKEN_COMMA, TOKEN_BRACKET_CLOSE, TOKEN_COMP_E,
				TOKEN_COMP_NE, TOKEN_COMP_GT, TOKEN_COMP_GTE, TOKEN_COMP_LT, TOKEN_COMP_LTE, TOKEN_PARENTHESE_CLOSE };
			error_recovery_multi(11, self_follow);
		}
	}
}
static void parse_EXPR_TAG() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_AR_ADD: {
			fprintf(yyout, "Rule EXPR_TAG ->  + TERM EXPR_TAG\n");
			match(TOKEN_AR_ADD);
			parse_TERM();
			parse_EXPR_TAG();
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
static void parse_TERM() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID:
		case TOKEN_INTEGER:
		case TOKEN_FLOAT:
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule TERM ->  FACTOR TERM_TAG\n");
			parse_FACTOR();
			parse_TERM_TAG();
			break;
		}
		default: {
			match_multi(4, TOKEN_ID, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_PARENTHESE_OPEN);
			self_follow = (eTOKENS[12]){ TOKEN_AR_ADD, TOKEN_SEMICOLON, TOKEN_CURLY_BRACE_CLOSE, TOKEN_COMMA, TOKEN_BRACKET_CLOSE,
				TOKEN_COMP_E, TOKEN_COMP_NE, TOKEN_COMP_GT, TOKEN_COMP_GTE, TOKEN_COMP_LT, TOKEN_COMP_LTE, TOKEN_PARENTHESE_CLOSE };
			error_recovery_multi(12, self_follow);
		}
	}
}
static void parse_TERM_TAG() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_AR_MUL: {
			fprintf(yyout, "Rule TERM_TAG ->  * FACTOR TERM_TAG\n");
			match(TOKEN_AR_MUL);
			parse_FACTOR();
			parse_TERM_TAG();
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
static void parse_FACTOR() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_ID: {
			fprintf(yyout, "Rule FACTOR ->  id FACTOR_TAG\n");
			match(TOKEN_ID);
			parse_FACTOR_TAG();
			break;
		}
		case TOKEN_INTEGER: {
			fprintf(yyout, "Rule FACTOR ->  int_num \n");
			match(TOKEN_INTEGER);
			break;
		}
		case TOKEN_FLOAT: {
			fprintf(yyout, "Rule FACTOR ->  float_num  \n");
			match(TOKEN_FLOAT);
			break;
		}
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule FACTOR ->  (EXPR)\n");
			match(TOKEN_PARENTHESE_OPEN);
			parse_EXPR();
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
}
static void parse_FACTOR_TAG() {
	eTOKENS* self_follow;
	nextToken = next_token();
	back_token();

	switch (nextToken->kind) {
		case TOKEN_BRACKET_OPEN: {
			fprintf(yyout, "Rule FACTOR_TAG ->  VAR_TAG\n");
			parse_VAR_TAG();
			break;
		}
		case TOKEN_PARENTHESE_OPEN: {
			fprintf(yyout, "Rule FACTOR_TAG ->  ( ARGS )\n");
			match(TOKEN_PARENTHESE_OPEN);
			parse_ARGS();
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
