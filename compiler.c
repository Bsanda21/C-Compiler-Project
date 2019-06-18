#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Constants */
#define FALSE 0
#define TRUE 1

/* Token Type */
typedef enum
{
	ID, INTLITERAL, MAIN, IF, ELSE, READ, WRITE, PLUSOP, MINUSOP,
	ASSIGNOP, LPAREN, RPAREN, COMMA, SEMICOLON, SCANEOF, LBRAC,
	RBRAC, MULTOP, DIVOP, LESSTHAN, GREATERTHAN, LESSEQUAL, GREATEQUAL,
	EQUALTO, NOTEQUAL, WHILE
	
} token;
/* Scanner Functions */
token scanner();
void clear_buffer();
void buffer_char(char c);
token check_reserved();
void lexical_error();

/* Parser Functions */
void parser();
void program();
void statement_list();
void statement();
void id_list();
void expression_list();
void expression();
void term();
void add_op();
void mult_op();
void match(token tok);
void syntax_error();
void boolean_expression();
void factor();

/* Global Variables */
FILE *fin; /* Source code file */
FILE *fout; /* Output token file */ 
token next_token; /* Next token in the source file */
char token_buffer[100]; /* Token buffer */
int token_ptr; /* Buffer pointer */
int line_num = 1; /* Current line number in source code file */
int error = FALSE; /* Lexical or Syntax error flag */
int main_choice = 0; /* Main method choice */

/********************************************************************************/

token scanner() {
	char c;
	clear_buffer();  /* Empty token buffer */
	while(TRUE){  /* Keep grabbing a character */
		c = getc(fin);
		if(c == EOF) { /* END OF FILE */
			if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
			fprintf(fout, "\nSCANEOF");
			}
			return SCANEOF;
		} else if (isspace(c)) { 
			if(c == '\n') {
				line_num = line_num + 1;
				if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
				fprintf(fout, "\n");
				}
			}
		} else if(isalpha(c)) { /* A WORD */
			buffer_char(c);
			c = getc(fin);
			while(isalnum(c) || c == '_') {
				buffer_char(c);
				c = getc(fin);
			}
			ungetc(c, fin);
			return check_reserved();
		} else if(isdigit(c)) {  /* A NUMBER */
			buffer_char(c);
			c = getc(fin);
			while(isdigit(c)) {
				buffer_char(c);
				c = getc(fin);
			}
			ungetc(c, fin); /* Unselect the current character */
			if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
				fprintf(fout, "INTLITERAL ");
			}
			return INTLITERAL;
		} else if(c == '(') { /* LEFT PARENTHESIS */
			if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
			fprintf(fout, "LPAREN ");
			}
			return LPAREN;
		} else if(c == ')') { /* RIGHT PARENTHESIS */
			if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
			fprintf(fout, "RPAREN ");
			}
			return RPAREN;
		} else if(c == '{') {  /* RIGHT BRACKET */
			if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
			fprintf(fout, "LBRAC ");
			}
			return LBRAC;
		} else if(c == '}') { /* LEFT BRACKET */
			if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
			fprintf(fout, "RBRAC ");
			}
			return RBRAC;
		} else if(c == ',') { /* COMMA */
			if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
			fprintf(fout, "COMMA ");
			}
			return COMMA;
		} else if(c == ';') { /* SEMICOLON */
			if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
			fprintf(fout, "SEMICOLON ");
			}
			return SEMICOLON;
		} else if(c == '+') { /* ADDITION */
			if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
			fprintf(fout, "PLUSOP ");
			}
			return PLUSOP;
		} else if(c == '*') { /* MULTIPLICAION */
			if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
			fprintf(fout, "MULTOP ");
			}
			return MULTOP;
		} else if(c == '-') { /* SUBTRACTION */
			if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
			fprintf(fout, "MINUSOP ");
			}
			return MINUSOP;
		}  else if(c == '/') {
			c = getc(fin);
			if(c == '/') { /* COMMENT */
			do
				c = getc(fin);
			while(c != '\n');
			line_num = line_num + 1;
			} else { /* DIVISION */
				if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
					fprintf(fout, "DIVOP ");
				}
				ungetc(c, fin); /* Unselect the current character */
				return DIVOP;
			}
		} else if(c == ':') {
			c = getc(fin);
			if(c == '=') {
				if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
				fprintf(fout, "ASSIGNOP ");
				}
				return ASSIGNOP;
			} else {
				ungetc(c, fin); /* Unselect the current character */
				lexical_error();
			}
		} else if(c == '<') { 
			c = getc(fin);
			if(c == '=') { /* LESS THAN OR EQUAL TO */
				if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
				fprintf(fout, "LESSEQUAL ");
				}
				return LESSEQUAL;
			} else { /* LESS THAN */
				ungetc(c, fin); /* Unselect the current character */
				fprintf(fout, "LESSTHAN ");
				return LESSTHAN;
			}
		} else if(c == '>') {
			c = getc(fin);
			if(c == '=') { /* GREATER THAN OR EQUAL TO */
				if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
				fprintf(fout, "GREATEQUAL ");
				}
				return GREATEQUAL;
			} else { /* GREATER THAN */
				ungetc(c, fin); /* Unselect the current character */
				if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
				fprintf(fout, "GREATERTHAN ");
				}
				return GREATERTHAN;
			}
		} else if(c == '=') {
			c = getc(fin);
			if(c == '=') { /* EQUAL TO */
				if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
				fprintf(fout, "EQUALTO ");
				}
				return EQUALTO;
			} else {
				ungetc(c, fin); /* Unselect the current character */
				lexical_error;
			}
		} else if(c == '!') {
			c = getc(fin);
			if(c == '=') { /* NOT EQUAL TO */
				if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
				fprintf(fout, "NOTEQUAL ");
				}
				return NOTEQUAL;
			} else {
				ungetc(c, fin);  /* Unselect the current character */
				lexical_error; /* Lexical error */
			}
		} else {
			lexical_error(); /* No tokens match, so must be a lexical error */
		}
	}
}
void clear_buffer() {
	token_ptr = 0; /* Resets the token pointer */
	token_buffer[token_ptr] = '\0'; /* Adds null to the end of the token string */
}
void buffer_char(char c) {
	token_buffer[token_ptr] = c; /* Append current character */
	token_ptr = token_ptr + 1; /* Move token pointer */
	token_buffer[token_ptr] = '\0'; /* Move null character */
} 
token check_reserved() {
	if(strcmp(token_buffer, "main") == 0) { /* MAIN token */
		if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
		fprintf(fout, "MAIN ");
		}
		return MAIN;
	} else if(strcmp(token_buffer, "if") == 0) { /* IF token */
		if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
		fprintf(fout, "IF ");
		}
		return IF;
	} else if(strcmp(token_buffer, "read") == 0) { /* READ token */
		if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
		fprintf(fout, "READ ");
		}
		return READ;
	} else if(strcmp(token_buffer, "write") == 0) { /* WRITE token */
		if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
		fprintf(fout, "WRITE ");
		}
		return WRITE; 
	} else if(strcmp(token_buffer, "else") == 0) { /* ELSE token */
		if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
		fprintf(fout, "ELSE ");
		}
		return ELSE;
	} else if(strcmp(token_buffer, "while") == 0) { /* WHILE token */
		if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
		fprintf(fout, "WHILE ");
		}
		return WHILE;
	} else { /* ID token */
		if(main_choice == 1) { /* Only print token to file if the user selected option 1 */
		fprintf(fout, "ID ");
		}
		return ID;
	}
}
void lexical_error() {
	printf("Lexical ERROR! line %d\n", line_num); /* Print lexical error with line number */
	error = TRUE; /* Set error flag */
	printf("To continue, please fix your code and run again!\n");
	exit(0); /* Exit program */
}

void parser() {
	next_token = scanner(); /* Get the first token */
	program(); /* Begin BNF */
	match(SCANEOF); /* Check for SCANEOF as the last token */
}
void program() {
	/* program --> main {<statement_list>} */
	match(MAIN);  /* Check for MAIN token */
	match(LBRAC); /* Check for LBRAC token */
	statement_list();
	match(RBRAC); /* Check for RBRAC */
}
void statement_list() {
	/* <statement_list> --> <statement>{,<statement>} */
	statement();  /* First statement */
	while(TRUE) {
		if(next_token == ID || next_token == READ || next_token == WRITE || next_token == WHILE || next_token == IF) {
			statement();  /* Possible second statement */
		} else {
			break;
		}
		
	}
}
void statement() {
	/* <statement> --> id:=<expression>; */
	/* <statement> --> read(<id_list>; */
	/* <statement> --> write(<expression_list>; */
	/* <statement> --> while(<boolean_expression>){<statement_list>} */
	/* <statement> --> if(<boolean_expression>){<statement_list>}*/
	/* <statement> --> if(<boolean_expression>){<statement_list>}else{<statement_list>} */
	if(next_token == ID) {
		match(ID);
		match(ASSIGNOP);
		expression();
		match(SEMICOLON);
	} else if(next_token == READ) {
		match(READ);
		match(LPAREN);
		id_list();
		match(RPAREN);
		match(SEMICOLON);
	} else if(next_token == WRITE) {
		match(WRITE);
		match(LPAREN);
		expression_list();
		match(RPAREN);
		match(SEMICOLON);
	} else if(next_token == WHILE) {
		match(WHILE);
		match(LPAREN);
		boolean_expression();
		match(RPAREN);
		match(LBRAC);
		statement_list();
		match(RBRAC);
	} else if(next_token == IF) {
		match(IF);
		match(LPAREN);
		boolean_expression();
		match(RPAREN);
		match(LBRAC);
		statement_list();
		match(RBRAC);
		if(next_token == ELSE) {
			match(ELSE);
			match(LBRAC);
			statement_list();
			match(RBRAC);
		}
	} else {
		syntax_error();
	}
	
}
void id_list() {
	/* <id_list> --> id{,id} */
	match(ID); /* Check for ID token */
	while(next_token == COMMA) {
		match(COMMA); /* Check for more COMMA token */
		match(ID); /* Check for more ID token */
	}
}
void expression_list() {
	 /* <expression_list> --> <expression>{, <expression>}	*/
	expression();  /* First expression */
	while(next_token == COMMA) {
		match(COMMA); /* Check for more COMMA token */
		expression();  /* Another possible expression */
	}
}
void expression() {
	/* <expression> --> <term>|<term>+<term>|<term>-<term> */
	term();
	while(next_token == PLUSOP || next_token == MINUSOP) {
		add_op();
		term();
	}
}
void term() {
	/* <term> --> <factor>|<factor>*<factor>|<factor>/<factor> */
	factor();
	while(next_token == MULTOP || next_token == DIVOP) {
		mult_op();
		factor();
	}
}
void factor() {
	/* <factor> --> id|integer|(<expression>) */
	if(next_token == ID) {
		match(ID);
	} else if(next_token == INTLITERAL) {
		match(INTLITERAL);
	} else if(next_token == LPAREN) {
		match(LPAREN);
		expression();
		match(RPAREN);
	} else {
		syntax_error();
	}
}
void add_op() {
	/* <add_op> --> +|- */
	if(next_token == PLUSOP || next_token == MINUSOP) {
		match(next_token);
	} else {
		syntax_error();
	}
}
void match(token tok) {
	if(tok == next_token)
		;
	else
		syntax_error();
	
	next_token = scanner();
}
void syntax_error() {
	printf("\nSyntax ERROR! Line %d\n", line_num);
	error = TRUE;
	printf("To continue, please fix your code and run again!\n");
	exit(0);
}
void boolean_expression() {
	/* <boolean_expression> --> integer|id        >|<|>=|<=|==|!=         integer|id*/
	if(next_token == ID || next_token == INTLITERAL) {
		match(next_token);
		
	} else {
		syntax_error();
	}
	if(next_token == LESSTHAN || next_token == LESSEQUAL || next_token == GREATERTHAN || next_token == GREATEQUAL || next_token == EQUALTO || next_token == NOTEQUAL) {
		match(next_token);
	} else {
		syntax_error();
	}
	if(next_token == ID || next_token == INTLITERAL) {
		match(next_token);
		
	} else {
		syntax_error();
	}
}
void mult_op() {
	/* <mult_op> --> *|/ */
	if(next_token == MULTOP || next_token == DIVOP) {
		match(next_token);
	} else {
		syntax_error();
	}
}

int main(void) 
{ 
	printf("Option (1): Scan your source code for lexical erros and Output all it's tokens\n");
	printf("Option (2): Parse your source code for lexical and syntax erros\n");
	printf("Please choose an option:");
	scanf("%d", &main_choice); /* Get user choice */
	if(main_choice == 1) { /* Option 1 */
		char source_code[30] = "source.txt";
		char output_source[30] = "output.txt";
		printf("Enter the name of your source code file:\n");
		scanf("%s", source_code); /* Get user source code file name */
		fin = fopen(source_code, "r"); /* Open the users source code file */
		if(fin == NULL) {  /* checks for null file */
			printf("Error finding or opening source code file\n");
			printf("Program exiting....\n");
			return 0;
		}
		printf("Enter the name of your output file\n");
		scanf("%s", output_source); /* Get user scanner output file name */
		fout = fopen(output_source, "r+"); /* Open the users output file */
		if(fout == NULL) {  /* checks for null file */
			printf("Error finding or opening output file\n");
			printf("Program exiting....\n");
			return 0;
		}
		while(TRUE){ /* Run scanner indefinitely */
			if(scanner() == SCANEOF) { /* When scanner returns END OF FILE exit the loop */
				break;
			}
		}
		fclose(fin); /* Close source code file */
		fclose(fout); /* Close output token file */
		printf("\nScanning Complete! No lexical errors!\n");
	} else if(main_choice == 2) { /* Option 2 */
		char source_code[30] = "source.txt";
		printf("Enter the name of your source code file:\n");
		scanf("%s", source_code); /* Get user source code file name */
		fin = fopen(source_code, "r"); /* Open source code file */
		if(fin == NULL) {  /* checks for null file */
			printf("Error finding or opening source code file\n");
			printf("Program exiting....\n");
			return 0;
		}
		parser(); /* Run the parser */
		fclose(fin); /* Close source code file */
		printf("\nParsing Complete! No lexical or syntax errors!\n");
	}
	return 0;
}