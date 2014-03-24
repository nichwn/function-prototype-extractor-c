#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#define CHAR_MAIN  4	/* number of characters in the function name, 'main' */

#define INIT_ARR  81	/* initial array size */
#define MULT_ARR   2	/* multiplication factor to increase array size */
#define COM_CHAR   2	/* characters required to initiate a comment */

#define COM_SINGLE 1	/* flag for a single line comment */
#define COM_MULTI  2	/* flag for a multi-line comment */

#define END	   1	/* return value for a function */

/****************************************************************/

/* function prototypes */
FILE *parse_comments(FILE *is, int c_type);
void parse(FILE *is);
FILE *skip_past_newline(FILE *is);
void parse_line(char *line);
int skip_past_whitespace(char *c_arr, int i);
char *check_alloc(char *items, int c_size, int *max_size);
int check_end_array(char *items, int pos);
void mem_chk(void *arr);
void skip_past_char(FILE *is, char c);

/****************************************************************/

/* skips through the input file until the end of the current comment has been
 * reached, where the comment type is indicated by the value of *c_type
 */
FILE *parse_comments(FILE *is, int c_type) {
	int c, aster;
	if (c_type == COM_SINGLE) {
		/* single line comment */
		is = skip_past_newline(is);
		return is;
	} else if (c_type == COM_MULTI) {
		/* multi-line comment */
		while ( 1 ) {
			c = fgetc(is);
			if (c == '*') {
				aster = 1;
			} else if (c == '/' && aster) {
				/* end of comment */
				return is;
			} else if (c == EOF) {
				/* non-terminating comment read */
				fprintf(stderr, "Error in parsing a multi-line "
					        "comment. Are you sure you "
					        "terminated your last comment "
					        "correctly?\n");
				exit(EXIT_FAILURE);
			} else {
				aster = 0;
			}
		}
	} else {
		fprintf(stderr, "An error occurred.\n");
		exit(EXIT_FAILURE);
	}
}

/****************************************************************/

/* parses a c file, is
 */
void parse(FILE *is) {
	char prev = '\0', curr;
	int linesize = INIT_ARR;
	int pos = 0, count = 0;
	char *line = malloc(linesize * sizeof(*line));
	mem_chk(line);
	
	while ((curr = fgetc(is)) != EOF) {
		line[pos] = curr;
		if (curr == '\'' || curr == '"') {
			/* string or character found */
			skip_past_char(is, curr);
		} else if (curr == '/' && prev == '/') {
			/* single line comment found */
			is = parse_comments(is, COM_SINGLE);
			pos = -1;
		} else if (curr == '*' && prev == '/') {
			/* multi-line comment found */
			is = parse_comments(is, COM_MULTI);
			pos -= COM_CHAR;
		} else if (curr == '{') {
			/* beginning of new block statement found */
			if (count == 0) {
				/* no nesting has occurred */
				line[pos + 1] = '\0';
				parse_line(line);
			}
			count++;
			pos = -1;
		} else if (curr == '}') {
			/* ending of a block statement found */
			count--;
			pos = -1;
			if (count < 0) {
				/* incorrect placement of block statements */
				fprintf(stderr, "Error encountered. Are you "
					        "sure you didn't attempt to "
					        "close a non-existent "
					        "block?\n");
				exit(EXIT_FAILURE);
			}
		} else if (curr == ';') {
			/* end of non-block statement found */
			pos = -1;
		} else if (curr == '#' && pos == 0) {
			/* pre-processor line found */
			is = skip_past_newline(is);
		}
		prev = curr;

		pos++;
		line = check_alloc(line, pos, &linesize);
	}
	
	if (count != 0) {
		/* incorrect placement of block statements */
		fprintf(stderr, "Error encountered. Are you sure you terminated "
			        "all of your blocks correctly?\n");
		exit(EXIT_FAILURE);
	}
	free(line);
}
			
/****************************************************************/

/* skips past all characters until a newline is found, or there is no
 * further input 
 */
FILE *skip_past_newline(FILE *is) {
	char c;
	while ((c = fgetc(is)) != '\n' && c != EOF);
	return is;
}

/****************************************************************/

/* determines whether the string is a function definition and if so, writes a
 * function prototype to stdout
 */
void parse_line(char *line) {
	int final_size = INIT_ARR;
	char *final = malloc(final_size * sizeof(*final));
	char sub[CHAR_MAIN];
	int i = 0, pos, para = 0;
	mem_chk(final);

	/* skip past whitespace */
	i = skip_past_whitespace(line, i);
	
	/* look for a return value */
	for (pos = 0; line[i] != '\0' && !isspace(line[i]); 
				i++, pos++) {
		final[pos] = line[i];
		final = check_alloc(final, pos, &final_size);
	}
	final[pos] = ' ';
	final = check_alloc(final, pos, &final_size);
	pos++;
	
	/* skip past whitespace */
	i = skip_past_whitespace(line, i);
	if (check_end_array(line, i) == END) {
		/* not a function definition */
		return;
	}
	
	/* look for the function name */
	for ( ; check_end_array(line, i) == !END && line[i] != '(';
				i++, pos++) {
		final[pos] = line[i];
		final = check_alloc(final, pos, &final_size);
	}
	strncpy(sub, final + pos - CHAR_MAIN, CHAR_MAIN);
	if (check_end_array(line, i) == END ||
				strncmp(sub, "main", CHAR_MAIN) == 0) {
		/* not a function definition, or is the main function */
		return;
	}

	/* look for the function parameters */
	for ( ; check_end_array(line, i) == !END && 
				(line[i] != ')' || para != 1);
				i++, pos++) {
		if (line[i] == '(') {
			para++;
		} else if (line[i] == ')') {
			para--;
		}
		final[pos] = line[i];
		final = check_alloc(final, pos, &final_size);
	}
	if (check_end_array(line, i) == END) {
		/* not a function definition */
		return;
	}
	final[pos] = ')';
	final = check_alloc(final, pos, &final_size);
	pos++;
	final[pos] = ';';
	final = check_alloc(final, pos, &final_size);
	pos++;
	final[pos] = '\0';
	
	/* function found, so write it */
	printf("%s\n", final);
	
	free(final);
}

/****************************************************************/

/* skips past all whitespace in a character array
 */
int skip_past_whitespace(char *c_arr, int i) {
	for( ; isspace(c_arr[i]) && check_end_array(c_arr, i) == !END; i++);
	return i;
}

/****************************************************************/

/* checks if there's space for another character in an array, and if not,
 * reallocates the memory
 */
char *check_alloc(char *items, int c_size, int *max_size) {
	if (c_size + 1 == *max_size) {
		*max_size *= MULT_ARR;
		items = realloc(items, *max_size * sizeof(*items));
		mem_chk(items);
	}
	return items;
}

/****************************************************************/

/* identifies whether the end of the array has been reached
 */
int check_end_array(char *items, int pos) {
	if (items[pos] == '\0') {
		return END;
	}
	return !END;
}

/****************************************************************/

/* checks that memory was allocated
 */
void mem_chk(void *arr) {
	if (arr == NULL) {
		fprintf(stderr, "Failure while allocating memory.\n");
		exit(EXIT_FAILURE);
	}
}

/****************************************************************/

/* skips to the next non-escaped, specified character 
 * (note: does not check if the escape sequence is standard) 
 */
void skip_past_char(FILE *is, char c) {
	int curr, count;
	for (curr = fgetc(is), count = 0; (curr != c || count % 2 != 0) &&
			curr != EOF; curr = fgetc(is)) {
		if (curr == '\\') {
			count++;
		} else {
			count = 0;
		}
	}
}

/****************************************************************/

int main(int argc, char *argv[]) {
	FILE *is = fopen(argv[1], "r");
	if (argc < 2) {
		/* no input file provided */
		fprintf(stderr, "Usage: function_prototype_extractor "
			"<input code file name> "
			"<output file [default = function_prototypes.txt]>\n");
			return EXIT_FAILURE;
	}

	if (is == NULL) {
		/* unable to open the file */
		fprintf(stderr, "Failed to open one or more files.\n");
		return EXIT_FAILURE;
	}
	parse(is);
	return 0;
}

