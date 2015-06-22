#include <stdio.h>
#include "awk.h"
#include "ytab.h"

struct xx
{	int token;
	char *name;
	char *pname;
} proc[] = {
	{ PROGRAM, "program", NULL },
	{ BOR, "boolop", " || " },
	{ AND, "boolop", " && " },
	{ NOT, "boolop", " !" },
	{ NE, "relop", " != " },
	{ EQ, "relop", " == " },
	{ LE, "relop", " <= " },
	{ LT, "relop", " < " },
	{ GE, "relop", " >= " },
	{ GT, "relop", " > " },
	{ ARRAY, "array", NULL },
	{ INDIRECT, "indirect", "$(" },
	{ SUBSTR, "substr", "substr" },
	{ SUB, "sub", "sub" },
	{ GSUB, "gsub", "gsub" },
	{ INDEX, "sindex", "sindex" },
	{ SPRINTF, "asprintf", "sprintf " },
	{ ADD, "arith", " + " },
	{ MINUS, "arith", " - " },
	{ MULT, "arith", " * " },
	{ DIVIDE, "arith", " / " },
	{ MOD, "arith", " % " },
	{ UMINUS, "arith", " -" },
	{ POWER, "arith", " **" },
	{ PREINCR, "incrdecr", "++" },
	{ POSTINCR, "incrdecr", "++" },
	{ PREDECR, "incrdecr", "--" },
	{ POSTDECR, "incrdecr", "--" },
	{ CAT, "cat", " " },
	{ PASTAT, "pastat", NULL },
	{ PASTAT2, "dopa2", NULL },
	{ MATCH, "matchop", " ~ " },
	{ NOTMATCH, "matchop", " !~ " },
	{ MATCHFCN, "matchop", "matchop" },
	{ INTEST, "intest", "intest" },
	{ PRINTF, "aprintf", "printf" },
	{ PRINT, "print", "print" },
	{ CLOSE, "closefile", "closefile" },
	{ DELETE, "delete", "delete" },
	{ SPLIT, "split", "split" },
	{ ASSIGN, "assign", " = " },
	{ ADDEQ, "assign", " += " },
	{ SUBEQ, "assign", " -= " },
	{ MULTEQ, "assign", " *= " },
	{ DIVEQ, "assign", " /= " },
	{ MODEQ, "assign", " %= " },
	{ POWEQ, "assign", " ^= " },
	{ CONDEXPR, "condexpr", " ?: " },
	{ IF, "ifstat", "if(" },
	{ WHILE, "whilestat", "while(" },
	{ FOR, "forstat", "for(" },
	{ DO, "dostat", "do" },
	{ IN, "instat", "instat" },
	{ NEXT, "jump", "next" },
	{ EXIT, "jump", "exit" },
	{ BREAK, "jump", "break" },
	{ CONTINUE, "jump", "continue" },
	{ RETURN, "jump", "ret" },
	{ BLTIN, "bltin", "bltin" },
	{ CALL, "call", "call" },
	{ ARG, "arg", "arg" },
	{ VARNF, "getnf", "NF" },
	{ GETLINE, "getline", "getline" },
	{ 0, "", "" },
};

#define SIZE	LASTTOKEN - FIRSTTOKEN + 1
char *table[SIZE];
char *names[SIZE];

main(argc,argv)
int argc;
char** argv;
{
	struct xx *p;
	int i, tok;
	char c;
	FILE *fp;
	char buf[100], name[100], def[100];

#if VMS     /* Or whatever the ANSI-X3J11-conforming word will be */
	extern char **cmd_lin();

        argv = cmd_lin( "", &argc, argv );
        /* The first argument provides misc. options (not currenly used). */
#endif

	printf("#include \"awk.h\"\n");
	printf("#include \"ytab.h\"\n\n");
	printf("Cell *nullproc();\n");
	for (i = SIZE; --i >= 0; )
		names[i] = "";
	for (p=proc; p->token!=0; p++)
		if (p == proc || strcmp(p->name, (p-1)->name))
			printf("extern Cell *%s();\n", p->name);

	if ((fp = fopen("ytab.h", "r")) == NULL) {
		fprintf(stderr, "maketab can't open ytab.h!\n");
		exit(1);
	}
	printf("static uchar *printname[%d] = {\n", SIZE);
	i = 0;
	while (fgets(buf, sizeof buf, fp) != NULL) {
		sscanf(buf, "%1c %s %s %d", &c, def, name, &tok);
		if (c != '#')	/* not #define */
			continue;
		if (tok < FIRSTTOKEN || tok > LASTTOKEN) {
			fprintf(stderr, "maketab funny token %d %s\n", tok, buf);
			exit(1);
		}
		names[tok-FIRSTTOKEN] = (char *) malloc(strlen(name)+1);
		strcpy(names[tok-FIRSTTOKEN], name);
		printf("\t(uchar *) \"%s\",\t/* %d */\n", name, tok);
		i++;
	}
	printf("};\n\n");

	for (p=proc; p->token!=0; p++)
		table[p->token-FIRSTTOKEN] = p->name;
	printf("\nCell *(*proctab[%d])() = {\n", SIZE);
	for (i=0; i<SIZE; i++)
		if (table[i]==0)
			printf("\tnullproc,\t/* %s */\n", names[i]);
		else
			printf("\t%s,\t/* %s */\n", table[i], names[i]);
	printf("};\n\n");

	printf("uchar *tokname(n)\n");	/* print a tokname() function */
	printf("{\n");
	printf("	static uchar buf[100];\n\n");
	printf("	if (n < FIRSTTOKEN || n > LASTTOKEN) {\n");
	printf("		sprintf(buf, \"token %%d\", n);\n");
	printf("		return buf;\n");
	printf("	}\n");
	printf("	return printname[n-257];\n");
	printf("}\n");
	exit(0);
}
