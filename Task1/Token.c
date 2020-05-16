#include "Token.h"

/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

int currentIndex = 0;
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
	Node* requiredNode = currentNode;
	int requiredIndex = currentIndex-1;
	if (requiredIndex == -1) {
		requiredNode = currentNode->prev;
		if (requiredNode == NULL) {
			// this is the start of the file and no previous token exists.
			return NULL;
		}
		requiredIndex = TOKEN_ARRAY_SIZE - 1;
	}
	return &(requiredNode->tokensArray[requiredIndex]); 
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
	while (currentNode == NULL) {
		if (yylex() == 0) {
			// yylex() has finished his work. no Node / Token was created
			// could happen if all of the input aren't in the language
			return NULL;
		}
	}
	// currentNode must exists.

	Node* requiredNode = currentNode;
	int requiredIndex = currentIndex + 1;
	Token* tempToken;
	if (requiredIndex < TOKEN_ARRAY_SIZE) {
		tempToken = &(requiredNode->tokensArray[requiredIndex]);
		if (tempToken->lexeme == 0) {
			// tokensArray is allocated using calloc which causes initialization of all of his Token structs to reset. (0 value insertion)
			// lexeme is empty.
			while (currentIndex != requiredIndex) {
				// run yylex until it finds the next legit token.
				if (yylex() == 0) {
					// yylex() finished with the file, no next token exists.
					return NULL;
				}
			}
		}
		// at this poiont, requiredIndex == currentIndex
	} else if (requiredIndex == TOKEN_ARRAY_SIZE) {
		requiredNode = currentNode->next;
		while (requiredNode != NULL) {
			// run yylex until it finds the next node is created for the next legit token.
			if (yylex() == 0) {
				// yylex() finished with the file, no next token exists.
				return NULL;
			}
		}
		requiredIndex = currentIndex;
		if (requiredIndex != 0) {
			printf("WTF  index should have gotten threw reset\n");
			return NULL;
		}
		// at this point requiredNode value shouldn't be NULL.
		tempToken = &(requiredNode->tokensArray[requiredIndex]);
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

int match(eTOKENS t) {
	Token* tok = next_token();
	if (tok != NULL) {
		return tok->kind == t;
	}
	return 0;
}

char* get_token_name(eTOKENS kind)
{
	return eTokenTitles[kind];
}