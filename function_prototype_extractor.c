#include <stdio.h>


#define INIT_ARR  81	/* initial array size */
#define MULT_ARR   2	/* multiplication factor to increase array size */
#define COM_CHAR   2	/* characters required to initiate a comment */

#define COM_NO     0	/* flag that indicates non-reading of a comment */
#define COM_SINGLE 1	/* flag for a single line comment */
#define COM_MULTI  2	/* flag for a multi-line comment */

#define ERROR     -1	/* error code */


/****************************************************************/

/* skips through the input file until the end of the current comment has been
 * reached, where the comment type is indicated by the value of *c_type
 */
FILE *parse_comments(FILE *is, int *c_type) {
	int c;
	if (*c_type == COM_SINGLE) {
		/* single line comment */
		while ((c = fgetc(is)) != '\n' && c != EOF);
		if (c == '\n') {
			/* end of comment */
			*c_type = COM_NO;
		} else {
			/* non-terminating comment read */
			*c_type = ERROR;
		}
		return is;
	} else if (*c_type == COM_MULTI) {
		/* multi-line comment */
		while ( 1 ) {
			while ((c = fgetc(is)) != '*' && c != EOF);
			if (fgetc(is) == '/') {
				/* end of comment */
				*c_type = COM_NO;
				return is;
			} else if (c == EOF) {
				/* non-terminating comment read */
				*c_type = ERROR;
				return is;
			}
		}
	} else {
		*c_type = ERROR;
		return is;
	}
}

/****************************************************************/

/* parses a c file, is
 */
int parse(FILE *is, FILE *os) {
	char prev = '', curr;
	int linesize = INIT_ARR;
	int pos = 0, count = 0;
	char *line = malloc(linesize * sizeof(*line));
	
	while ((curr = fgetc(is)) != EOF) {
		line[pos] = curr;
		if (curr == '/' && prev == '/') {
			/* single line comment found */
			is = parse_comments(is, COM_SINGLE);
			pos -= COM_CHAR;
		} else if (curr == '*' && prev == '/') {
			/* multi-line comment found */
			is = parse_comments(is, COM_MULTI);
			pos -= COM_CHAR;
		} else if (curr == '{' && count = 0) {
			/* beginning of new block statement found and no 
			 * nesting has occurred 
			 */
			count++;
			line[pos + 1] = '\0';
			pos = -1;
			// TODO - parse current line to determine if it's a function - then if so, outputs it
		} else if (curr == '}') {
			/* ending of a block statement found */
			count--;
			pos = -1;
			if (count < 0) {
				/* incorrect placement of block statements */
				return ERROR;
			}
		} else if (curr == ';') {
			/* end of non-block statement found */
			pos = -1;
		} else if (curr == '#' && pos == 0) {
			/* pre-processor line found */
			// TODO - insert function here which skips to the next line - just like in parse_comments
		}
		prev = curr;

		pos++;
		if (pos + 1 == linesize) {
			/* insufficient space to store a statement */
			linesize *= MULT_ARR;
			realloc(line, linesize * sizeof(*line));
		}
	
	if (count != 0) {
		/* incorrect placement of block statements */
		return ERROR;
	}
	return !ERROR;
}

