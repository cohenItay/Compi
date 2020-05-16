#include "Token.h"
#include <stdarg.h>

/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

int currentIndex = -1;
Node* currentNode = NULL;

#define TOKEN_ARRAY_SIZE 1000

/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{ 
	int length = strlen(lexeme)+1;
	
	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if(currentNode == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->tokensArray = 
			(Token*) calloc(sizeof(Token),TOKEN_ARRAY_SIZE);
		if(currentNode->tokensArray == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
		currentIndex = 0;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if(currentNode == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray = 
			 (Token*)calloc(sizeof(Token),TOKEN_ARRAY_SIZE);

			if(currentNode->tokensArray == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next = NULL;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}

	currentNode->tokensArray[currentIndex].kind = kind;	
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;

	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char)*length);
	#ifdef _WIN32
		strcpy_s(currentNode->tokensArray[currentIndex].lexeme, length, lexeme);
	#else
		strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
	#endif		
}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
* While updating currentIndex and currentNode appropriatley.
*/
Token* back_token() {
	int requiredIndex = currentIndex-1;
	if (requiredIndex == -1) {
		if (currentNode->prev == NULL) {
			// this is the start of the file and no previous token exists.
			return NULL;
		}
		requiredIndex = TOKEN_ARRAY_SIZE - 1;
	}
	currentIndex = requiredIndex;
	return &(currentNode->tokensArray[requiredIndex]); 
}

Token* current_token() {
	return &(currentNode->tokensArray[currentIndex]);
}

/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token): 
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/
Token* next_token() 
{
	Token* tempToken;
	int requiredIndex = currentIndex + 1;
	while (currentNode == NULL) {
		if (yylex() == 0) {
			tempToken = &(currentNode->tokensArray[currentIndex]);
			// yylex() has finished his work. should return TOKEN_EOF
			// could happen if all of the input aren't in the language
			return tempToken;
		}
	}
	// currentNode must exists.

	if (requiredIndex < TOKEN_ARRAY_SIZE) {
		tempToken = &(currentNode->tokensArray[requiredIndex]);
		if (tempToken->kind == TOKEN_NULL) {
			// tokensArray is allocated using calloc which causes initialization of all of his Token structs to reset. (0 value insertion)
			// lexeme is empty.
			while (currentIndex != requiredIndex) {
				// run yylex until it finds the next legit token.
				if (yylex() == 0) {
					// yylex() finished with the file, no next token exists. should return TOKEN_EOF
					return tempToken;
				}
			}
		}
		// at this poiont, requiredIndex == currentIndex
	} else if (requiredIndex == TOKEN_ARRAY_SIZE) {
		while (currentIndex != 0) {
			// run yylex until it finds the next node and resets the index for the next legit token.
			if (yylex() == 0) {
				// yylex() finished with the file, no next token exists.should return EOF
				return &(currentNode->tokensArray[currentIndex]);
			}
		}
		requiredIndex = currentIndex;
		if (requiredIndex != 0) {
			printf("WTF  index should have gotten threw reset\n");
			return NULL;
		}
		// at this point currentNode value shouldn't be NULL.
		tempToken = &(currentNode->tokensArray[requiredIndex]);
	}
	else {
		printf("Something went wrong with the indexes offset..\n");
		return NULL;
	}

	if (tempToken->lexeme == 0) {
		printf("WTF  token should have a lexeme pointer which is non-null\n");
		return NULL;
	}
	return tempToken;
}

static char* concat(const char* s1, const char* s2)
{
	const size_t len1 = strlen(s1);
	const size_t len2 = strlen(s2);
	char* result = malloc(len1 + len2 + 1); // +1 for the null-terminator
	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
	return result;
}

/*
* match to one of the eTOKENS sent.
*/
int match(int count, eTOKENS t, ...) {
	Token* tok = next_token();
	int isMatch;
	va_list tkns_ap;
	int i;
	char* requiredTokensNames;
	int freeStr = 0;

	if (tok != NULL) {
		va_start(tkns_ap, count); // initialize the argument list
		t = va_arg(tkns_ap, eTOKENS);
		isMatch = tok->kind == t;
		requiredTokensNames = get_token_name(t);
		if (!isMatch && count > 1) {
			// search if other token matches.
			for (i = 1; i < count; i++) {
				t = va_arg(tkns_ap, eTOKENS);
				isMatch = tok->kind == t;
				if (isMatch) {
					break;
				}
				else {
					freeStr = 1;
					requiredTokensNames = concat(requiredTokensNames, " | ");
					requiredTokensNames = concat(requiredTokensNames, get_token_name(t));
				}
			}
		}
		if (!isMatch) {
			fprintf(
				yyout, 
				"Expected token of type '%s' at line: %d, Actual token of type '%s', lexeme: '%s'.",
				requiredTokensNames,
				tok->lineNumber,
				get_token_name(tok->kind),
				tok->lexeme
			);
		}
		if (freeStr)
			free(requiredTokensNames);
		return isMatch;
	}
	printf("match func: next_token() returned NULL, WTF\n");
	return 0;
}

char* get_token_name(eTOKENS kind)
{
	return eTokenTitles[kind];
}