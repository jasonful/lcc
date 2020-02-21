/* x86s running Linux */

#include <string.h>
#include <stdio.h>

static char rcsid[] = "$Id$";

char *suffixes[] = { ".c", ".i", ".asm;.ASM;.s;.S", ".o", ".out", 0 };
char inputs[256] = "";
char *cpp[] = { "cpp", "$1", "$2", "$3", 0 };
char *include[] = { 0 };
char *com[] = { "rcc", "$1", "$2", "$3", 0 };
char *as[] = {""};
char *ld[] = {""};

extern char *concat(char *, char *);
extern char *replace(const char *, int, int);

int option(char *arg) {
	if (strncmp(arg, "-lccdir=", 8) == 0) {
		if (arg[strlen(arg)-1] != '/') {
			cpp[0] = concat(&arg[8], "/cpp");
			com[0] = concat(&arg[8], "/rcc");
		}
		else {
			cpp[0] = concat(&arg[8], "cpp");
			com[0] = concat(&arg[8], "rcc");
		}
		return 2;
	}
	else if (strcmp(arg, "-g") == 0)
		;
	else if (strcmp(arg, "-p") == 0)
		;
	else if (strcmp(arg, "-b") == 0)
		;
	else
		return 0;
	return 1;
}
