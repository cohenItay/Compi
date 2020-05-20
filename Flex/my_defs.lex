%option noyywrap

%{
	#pragma warning(disable : 4996)
	#include "Token.h"
	#include "parser.h"
	#define TEMAMTES_INFO "205651078_304950702"
	#define COPYRIGHTS "Created by Tom Naidich and Itay Cohen"
	int line_num = 1;
	int print_lex = 1;
%}

/* declare on comment related states */
%x 										commentTypeMultiLine commentTypeSingleLine
DIGIT 									[0-9]
ALPHA									[a-zA-Z]
ALPHA_UPPER								[A-Z]
ALPHA_LOWER								[a-z]

%%

"/*"  									{ BEGIN(commentTypeMultiLine); } /* Handle Multi Line Comments */
<commentTypeMultiLine>"*/"  			{ BEGIN(0); }
<commentTypeMultiLine>.            		/* Do nothing until comment region is closed */
<commentTypeMultiLine>\n             	{ line_num++; }
<commentTypeMultiLine><<EOF>>           { return handle_unexpected_eof(); }

"//"   									{ BEGIN(commentTypeSingleLine); } /* Handle Single Line Comments */
<commentTypeSingleLine>.             	/* Do nothing untill a new line is reached */
<commentTypeSingleLine>\n   			{ line_num++; BEGIN(0); }

"void"									{ return handle_token(TOKEN_VOID_TYPE); }
"int"									{ return handle_token(TOKEN_INTEGER_TYPE); }
"float"									{ return handle_token(TOKEN_FLOAT_TYPE); }

"if"									{ return handle_token(TOKEN_IF); }
"return"								{ return handle_token(TOKEN_RETURN); }

"=="									{ return handle_token(TOKEN_COMP_E); }
"!="									{ return handle_token(TOKEN_COMP_NE); }
">"										{ return handle_token(TOKEN_COMP_GT); }
">="									{ return handle_token(TOKEN_COMP_GTE); }
"<"										{ return handle_token(TOKEN_COMP_LT); }
"<="									{ return handle_token(TOKEN_COMP_LTE); }

"+"										{ return handle_token(TOKEN_AR_ADD); }
"*"										{ return handle_token(TOKEN_AR_MUL); }
"="										{ return handle_token(TOKEN_AR_EQUAL); }

","										{ return handle_token(TOKEN_COMMA); }
":"										{ return handle_token(TOKEN_COLON); }
";"										{ return handle_token(TOKEN_SEMICOLON); }

"("										{ return handle_token(TOKEN_PARENTHESE_OPEN); }
")"										{ return handle_token(TOKEN_PARENTHESE_CLOSE); }
"["										{ return handle_token(TOKEN_BRACKET_OPEN); }
"]"										{ return handle_token(TOKEN_BRACKET_CLOSE); }
"{"										{ return handle_token(TOKEN_CURLY_BRACE_OPEN); }
"}"										{ return handle_token(TOKEN_CURLY_BRACE_CLOSE); }

{ALPHA_LOWER}+(_?({ALPHA}|{DIGIT}))*									{ return handle_token(TOKEN_ID); }			 /* Id group */
(0|[1-9]{DIGIT}*).{DIGIT}+[eE](\+|\-)?(0|[1-9]{DIGIT}*)					{ return handle_token(TOKEN_FLOAT); }		 /* Handle Float values */
0|[1-9]{DIGIT}*															{ return handle_token(TOKEN_INTEGER); }		/* Handle Integer values */


<<EOF>> 								{ return handle_token(TOKEN_EOF); } /* Handle END OF FILE */

" "										/* Do nothing */
"\t"									/* Do nothing */
\n   									{ line_num++; }
.										{ return handle_invalid_token(); }
%%

int handle_token(eTOKENS token_kind) 
{
	create_and_store_token(token_kind, yytext, line_num);
	
	char* token_name = get_token_name(token_kind);
	if (print_lex)
		fprintf(yyout, "Token of type '%s', lexeme: '%s', found in line: %d.\n", token_name, yytext, line_num);

	return token_kind != TOKEN_EOF ? 1 : 0;
}

int handle_invalid_token() 
{
	if (print_lex)
		fprintf(yyout, "- ERROR: The character '%s' at line: %d does not begin any legal token in the language.\n", yytext, line_num);
	return 1;
}

int handle_unexpected_eof()
{
	if (print_lex)
		fprintf(yyout, "- ERROR: Reached EOF unexpectedly at line: %d.", line_num);
	return 0;
}

void main(int argc, char* argv[])
{
	const char* path_input1 = "C:\\temp\\test1.txt";
	const char* path_input2 = "C:\\temp\\test2.txt";
	

	/* ----------------- Lexical start ---------------------- */
	const char* path_output1_lex = "C:\\temp\\test1_" TEMAMTES_INFO "_lex.txt";
	const char* path_output2_lex = "C:\\temp\\test2_" TEMAMTES_INFO "_lex.txt";

	// HANDLE TestCase 1
	yyin = fopen(path_input1, "r");
	yyout = fopen(path_output1_lex, "w");
	
	if (yyin == NULL || yyout == NULL) {
		const char* invalid_path = yyin == NULL ? path_input1 : path_output1_lex;
		printf("\nERROR: Failed to open file at path %s. Aborting...", invalid_path);
    } else {
		while (yylex() != 0);
		yyrestart(yyin);
		yyrestart(yyout);
		fclose(yyin);
		fclose(yyout);
		printf("INFO: Output for test file 1 lex has been generated successfully.\n");
	}
	line_num = 1;
	// END OF TestCase 1
	

	
	// HANDLE TestCase 2
	yyin = fopen(path_input2, "r");
	yyout = fopen(path_output2_lex, "w");
	
	
	if (yyin == NULL || yyout == NULL) {
		const char* invalid_path = yyin == NULL ? path_input2 : path_output2_lex;
        printf("ERROR: Failed to open file at path %s. Aborting...\n", invalid_path);
    } else {
		yyrestart(yyin);
		while(yylex() != 0);
		yyrestart(yyout);
		fclose(yyin);
		fclose(yyout);
		printf("INFO: Output for test file 2 lex has been generated successfully.\n\n");
	}
	line_num = 1;
	// END OF TestCase 2
	/* -------------------- Lexical end --------------------- */



	print_lex = 0;


	/* ----------------- Syntactic start ---------------------- */
	const char* path_output1_syntactic = "C:\\temp\\test1_" TEMAMTES_INFO" _syntactic.txt";
	const char* path_output2_syntactic = "C:\\temp\\test2_" TEMAMTES_INFO" _syntactic.txt";

	// HANDLE TestCase 1
	yyin = fopen(path_input1, "r");
	yyout = fopen(path_output1_syntactic, "w");
	
	if (yyin == NULL || yyout == NULL) {
		const char* invalid_path = yyin == NULL ? path_input1 : path_output1_syntactic;
		printf("\nERROR: Failed to open file at path %s. Aborting...", invalid_path);
    } else {
		parse();
		yyrestart(yyin);
		yyrestart(yyout);
		fclose(yyin);
		fclose(yyout);
		printf("INFO: Output for test file 1 syntactic has been generated successfully.\n");
	}
	line_num = 1;
	// END OF TestCase 1
	

	

	// HANDLE TestCase 2
	yyin = fopen(path_input2, "r");
	yyout = fopen(path_output2_syntactic, "w");
	
	if (yyin == NULL || yyout == NULL) {
		const char* invalid_path = yyin == NULL ? path_input2 : path_output2_syntactic;
        printf("\nERROR: Failed to open file at path %s. Aborting...", invalid_path);
        return;
    } else {
		parse();
		yyrestart(yyin);
		yyrestart(yyout);
		fclose(yyin);
		fclose(yyout);
		printf("INFO: Output for test file 2 syntactic has been generated successfully.\n");
	}
	line_num = 1;
	// END OF TestCase 2
	/* -------------------- Syntactic end --------------------- */
	
}
