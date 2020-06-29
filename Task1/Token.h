#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin, *yyout;

typedef enum eTOKENS
{
	//Undefined token
	TOKEN_NULL,
	// Return Types
	TOKEN_VOID_TYPE, TOKEN_INTEGER_TYPE, TOKEN_FLOAT_TYPE,
	// Variable Types
	TOKEN_INTEGER, TOKEN_FLOAT,
	// Statement Types
	TOKEN_IF, TOKEN_RETURN,
	// Assignment Operation
	TOKEN_AR_ADD, TOKEN_AR_MUL, TOKEN_AR_EQUAL, 
	// Comparison Operations
	TOKEN_COMP_E, TOKEN_COMP_NE, TOKEN_COMP_GT, TOKEN_COMP_GTE, TOKEN_COMP_LT, TOKEN_COMP_LTE,
	// Separation Signs
	TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON,
	// Parenthese
	TOKEN_PARENTHESE_OPEN, TOKEN_PARENTHESE_CLOSE, 
	// Bracket
	TOKEN_BRACKET_OPEN, TOKEN_BRACKET_CLOSE,
	// Curly Braces
	TOKEN_CURLY_BRACE_OPEN, TOKEN_CURLY_BRACE_CLOSE,
	// General Tokens
	TOKEN_ID, TOKEN_EOF
}eTOKENS;

static const char* eTokenTitles[] = {
	//Undefined token
	"NULL",
	// Return Types
	"VoidType", "IntType", "FloatType",
	// Variable Types
	"Int", "Float",
	// Statement Types
	"IfStatement", "ReturnStatement",
	// Assignment Operation
	"AddAr", "MulAr", "EqualAr",
	// Comparison Operations
	"EqaualComp", "NotEqaualComp", "GreaterThanComp", "GreaterThanEqaualComp", "LessThanComp", "LessThanEqaualComp",
	// Separation Signs
	"Comma", "Colon", "Semicolon",
	// Parenthese
	"ParentheseOpen", "ParentheseClose",
	// Bracket
	"BracketOpen", "BracketClose",
	// Curly Braces
	"CurlyBraceOpen", "CurlyBraceClose",
	// General Tokens
	"ID", "EOF"
};

typedef struct Token
{
	eTOKENS kind;
	char *lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token *tokensArray;
	struct Node *prev;
	struct Node *next;
} Node;

void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token *next_token();
Token* current_token();
Token *back_token();
void setParserTokenPtr(Token** tkn);
int match(eTOKENS t);
int match_multi(int count, eTOKENS t, ...);

char* get_token_name(eTOKENS kind);
#endif