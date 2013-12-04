#include <stdio.h>


#define COM_NO     0	/* flag that indicates non-reading of a comment */
#define COM_SINGLE 1	/* flag for a single line comment */
#define COM_MULTI  2	/* flag for a multi-line comment */

#define ERROR     -1	/* error code */


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

