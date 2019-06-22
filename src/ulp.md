%{
/*
	Copyright 2019 Jason Fuller

    This file is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program in the doc/COPYING file.
	If not, see <https://www.gnu.org/licenses/>.

	This notice applies only to this file.  Most of lcc is governed by
	a different license which can be found in the CPYRIGHT file in the
	root directory of the source code.
*/
/* Don't use r3 because it is reserved for handling spills */
#define TMASKIREG 0x7
/* We don't have enough registers for register variables */
#define VMASKIREG 0x0

#define RMASKIREG 0x1

#include "c.h"
#define NODEPTR_TYPE Node
#define OP_LABEL(p) ((p)->op)
#define LEFT_CHILD(p) ((p)->kids[0])
#define RIGHT_CHILD(p) ((p)->kids[1])
#define STATE_LABEL(p) ((p)->x.state)
static void address(Symbol, Symbol, long);
static void blkfetch(int, int, int, int);
static void blkloop(int, int, int, int, int, int[]);
static void blkstore(int, int, int, int);
static void defaddress(Symbol);
static void defconst(int, int, Value);
static void defstring(int, char *);
static void defsymbol(Symbol);
static void doarg(Node);
static void emit2(Node);
static void export(Symbol);
static void clobber(Node);
static void function(Symbol, Symbol [], Symbol [], int);
static void global(Symbol);
static void import(Symbol);
static void local(Symbol);
static void progbeg(int, char **);
static void progend(void);
static void segment(int);
static void space(int);
static void target(Node);
extern int ckstack(Node, int);
extern int memop(Node);
extern int sametree(Node, Node);

static Symbol ireg[32];
static Symbol iregw;
static int cseg;
static char* currentfunction;

%}
%start stmt

%term CNSTF4=4113
%term CNSTI4=4117
%term CNSTP4=4119
%term CNSTU4=4118

%term ARGB=41
%term ARGF4=4129
%term ARGI4=4133
%term ARGP4=4135
%term ARGU4=4134

%term ASGNB=57
%term ASGNF4=4145
%term ASGNI4=4149
%term ASGNP4=4151
%term ASGNU4=4150

%term INDIRB=73
%term INDIRF4=4161
%term INDIRI4=4165
%term INDIRP4=4167
%term INDIRU4=4166

%term CVFF4=4209
%term CVFI4=4213

%term CVIF4=4225
%term CVII4=4229
%term CVIU4=4230

%term CVPU4=4246

%term CVUI4=4277
%term CVUP4=4279
%term CVUU4=4278

%term NEGF4=4289
%term NEGI4=4293

%term CALLB=217
%term CALLF4=4305
%term CALLI4=4309
%term CALLP4=4311
%term CALLU4=4310
%term CALLV=216

%term RETF4=4337
%term RETI4=4341
%term RETP4=4343
%term RETU4=4342
%term RETV=248

%term ADDRGP4=4359

%term ADDRFP4=4375

%term ADDRLP4=4391

%term ADDF4=4401
%term ADDI4=4405
%term ADDP4=4407
%term ADDU4=4406

%term SUBF4=4417
%term SUBI4=4421
%term SUBP4=4423
%term SUBU4=4422

%term LSHI4=4437
%term LSHU4=4438

%term MODI4=4453
%term MODU4=4454

%term RSHI4=4469
%term RSHU4=4470

%term BANDI4=4485
%term BANDU4=4486

%term BCOMI4=4501
%term BCOMU4=4502

%term BORI4=4517
%term BORU4=4518

%term BXORI4=4533
%term BXORU4=4534

%term DIVF4=4545
%term DIVI4=4549
%term DIVU4=4550

%term MULF4=4561
%term MULI4=4565
%term MULU4=4566

%term EQF4=4577
%term EQI4=4581
%term EQU4=4582

%term GEF4=4593
%term GEI4=4597
%term GEU4=4598

%term GTF4=4609
%term GTI4=4613
%term GTU4=4614

%term LEF4=4625
%term LEI4=4629
%term LEU4=4630

%term LTF4=4641
%term LTI4=4645
%term LTU4=4646

%term NEF4=4657
%term NEI4=4661
%term NEU4=4662

%term JUMPV=584

%term LABELV=600

%term VREGP=711
%term LOADI4=4325
%term LOADU4=4326
%term LOADP4=4327

%%
reg:  INDIRI4(VREGP)     "# read register\n"
reg:  INDIRU4(VREGP)     "# read register\n"
reg:  INDIRP4(VREGP)     "# read register\n"
stmt: ASGNI4(VREGP,reg)  "# write register\n"
stmt: ASGNU4(VREGP,reg)  "# write register\n"
stmt: ASGNP4(VREGP,reg)  "# write register\n"
con: CNSTI4  "%a"
con: CNSTU4  "%a"
zero: CNSTI4 "# compare to zero" range(a,0,0)
zero: CNSTU4 "# compare to zero" range(a,0,0)
stmt: reg  ""
acon: con     "%0"
acon: ADDRGP4 "%a"
acon: ADDRLP4 "%a"
addr: acon  "%0"
reg:  addr  "move %c,%0\n"  1
spill: ADDRLP4 "%a"
stmt: ASGNI4(spill, reg) "move r3,%0\nst %1,r3,0\n" 2
stmt: ASGNU4(spill, reg) "move r3,%0\nst %1,r3,0\n" 2
stmt: ASGNP4(spill, reg) "move r3,%0\nst %1,r3,0\n" 2
stmt: ASGNI4(reg,reg)  "st %1,%0,0\n"  1
stmt: ASGNU4(reg,reg)  "st %1,%0,0\n"  1
stmt: ASGNP4(reg,reg)  "st %1,%0,0\n"  1
reg:  INDIRI4(reg)     "ld %c,%0,0\n"  1
reg:  INDIRU4(reg)     "ld %c,%0,0\n"  1
reg:  INDIRP4(reg)     "ld %c,%0,0\n"  1
reg:  INDIRF4(addr)     ".error \x22 float not supported\x22 \n"  LBURG_MAX
stmt: ASGNF4(addr,reg)  ".error \x22 float not supported\x22 \n"  LBURG_MAX
reg: DIVI4(reg,reg)  ".error \x22 Division not supported.  Try right shift >>\x22 \n"   LBURG_MAX
reg: DIVU4(reg,reg)  ".error \x22 Division not supported.  Try right shift >>\x22 \n"  LBURG_MAX
reg: MODI4(reg,reg)  ".error \x22 Mod not supported\x22 \n"   LBURG_MAX
reg: MODU4(reg,reg)  ".error \x22 Mod not supported\x22 \n"  LBURG_MAX
reg: MULI4(reg,reg)  ".error \x22 Multiplication not supported. Try left shift <<\x22 \n"   LBURG_MAX
reg: MULU4(reg,reg)  ".error \x22 Multiplication not supported. Try left shift <<\x22 \n"   LBURG_MAX
rc:  con            "%0"
rc:  reg            "%0"
reg: ADDI4(reg,rc)   ".error \x22 Signed addition not supported.  Use unsigned.\x22 \n"  LBURG_MAX
reg: ADDP4(reg,rc)   "add %c,%0,%1\n"  1
reg: ADDU4(reg,rc)   "add %c,%0,%1\n"  1
reg: BANDI4(reg,rc)  "and %c,%0,%1\n"  1
reg: BORI4(reg,rc)   "or  %c,%0,%1\n"  1
reg: BXORI4(reg,rc)  "and r3,%0,%1 # { %c = %0 ^ %1\nadd %0,%0,%1\nsub %0,%0,r3\nsub %c,%0,r3 # }\n" 4
reg: BANDU4(reg,rc)  "and %c,%0,%1\n"  1
reg: BORU4(reg,rc)   "or %c,%0,%1\n"   1
reg: BXORU4(reg,rc)  "and r3,%0,%1 # { %c = %0 ^ %1\nadd %0,%0,%1\nsub %0,%0,r3\nsub %c,%0,r3 # }\n" 4
reg: SUBI4(reg,rc)   ".error \x22 Signed subtraction not supported.  Use unsigned.\x22 \n"  LBURG_MAX
reg: SUBP4(reg,rc)   "sub %c,%0,%1\n"  1
reg: SUBU4(reg,rc)   "sub %c,%0,%1\n"  1
rc16bit: CNSTU4         "%a"  range(a,0,0xFFFF)
rc16bit: CNSTI4         "%a"  range(a,0,0xFFFF)
rc16bit: reg            "%0"
reg: LSHI4(reg,rc16bit)  "lsh %c,%0,%1\n"  1
reg: LSHU4(reg,rc16bit)  "lsh %c,%0,%1\n"  1
reg: RSHI4(reg,rc16bit)  ".error \x22 Signed right-shift not supported.  Use unsigned.\x22 \n"  LBURG_MAX
reg: RSHU4(reg,rc16bit)  "rsh %c,%0,%1\n"  1
reg: BCOMI4(reg)  "move r3,0 # {%c = ~%0\nsub %c,r3,%0\nsub %c,%c,1 # }\n"   3
reg: BCOMU4(reg)  "move r3,0 # {%c = ~%0\nsub %c,r3,%0\nsub %c,%c,1 # }\n"   3
reg: NEGI4(reg)   "move r3,0\nsub %c,r3,%0\n"  2
reg: LOADI4(reg)  "move %c,%0\n"  move(a)
reg: LOADU4(reg)  "move %c,%0\n"  move(a)
reg: ADDF4(reg,reg)  ".error \x22 floating point addition not supported\x22 \n"       LBURG_MAX
reg: DIVF4(reg,reg)  ".error \x22 floating point division not supported\x22 \n"       LBURG_MAX
reg: MULF4(reg,reg)  ".error \x22 floating point multiplication not supported\x22 \n" LBURG_MAX
reg: SUBF4(reg,reg)  ".error \x22 floating point subtraction not supported\x22 \n"    LBURG_MAX
reg: NEGF4(reg)      ".error \x22 floating point negation not supported\x22 \n"       LBURG_MAX
reg: CVII4(reg)  "?move %c,%0\n"  move(a)
reg: CVUI4(reg)  "?move %c,%0\n"  move(a)
reg: CVUU4(reg)  "?move %c,%0\n"  move(a)
reg: CVFF4(reg)  ".error \x22 floating point not supported\x22 \n"  LBURG_MAX
reg: CVIF4(reg)  ".error \x22 floating point not supported\x22 \n"  LBURG_MAX
reg: CVFI4(reg)  ".error \x22 floating point not supported\x22 \n"  LBURG_MAX
stmt: LABELV  "%a:\n"
stmt: JUMPV(acon)  "jump %0\n" 1
stmt: JUMPV(reg)   "jump %0\n" 1
stmt: EQI4(reg,reg)  "sub %0,%0,%1 #{ if %0==%1 goto %a \njump 1f, eq\nadd %0,%0,%1\njump 2f\n1:\nadd %0,%0,%1\njump %a\n2:           #}\n" 6
stmt: EQU4(reg,reg)  "sub %0,%0,%1 #{ if %0==%1 goto %a \njump 1f, eq\nadd %0,%0,%1\njump 2f\n1:\nadd %0,%0,%1\njump %a\n2:           #}\n" 6
stmt: EQI4(reg,zero) "move %0,%0 #if %0 == 0 goto %a \njump %a, eq\n"  2
stmt: EQU4(reg,zero) "move %0,%0 #if %0 == 0 goto %a\njump %a, eq\n"   2
stmt: GEI4(reg,reg)  ".error \x22 Signed greater-than-or-equal not supported.  Use unsigned.\x22 \n"   LBURG_MAX
stmt: GEU4(reg,reg)  "sub %1,%1,%0 #{ if %0 >= %1 goto %a\nadd %1,%1,%0\njump %a, eq\njump %a, ov #}\n"  4
stmt: GTI4(reg,reg)  ".error \x22 Signed greater-than not supported.  Use unsigned.\x22 \n"   LBURG_MAX
stmt: GTU4(reg,reg)  "sub %1,%1,%0 #{ if %0 > %1 goto %a\nadd %1,%1,%0\njump %a, ov #}\n"  3
stmt: LEI4(reg,reg)  ".error \x22 Signed less-than-or-equal not supported.  Use unsigned.\x22 \n"   LBURG_MAX
stmt: LEU4(reg,reg)  "sub %0,%0,%1 #{ if %0 <= %1 goto %a\nadd %0,%0,%1\njump %a, eq\njump %a, ov #}\n"  3
stmt: LTI4(reg,reg)  ".error \x22 Signed less-than not supported.  Use unsigned.\x22 \n"   LBURG_MAX
stmt: LTU4(reg,reg)  "sub %0,%0,%1 #{ if %0 < %1 goto %a\nadd %0,%0,%1\njump %a, ov #}\n"  3
stmt: NEI4(reg,reg)  "sub %0,%0,%1 #{ if %0!=%1 goto %a \njump 1f, eq\nadd %0,%0,%1\njump %a\n1:\nadd %0,%0,%1 #}\n" 5
stmt: NEU4(reg,reg)  "sub %0,%0,%1 #{ if %0!=%1 goto %a \njump 1f, eq\nadd %0,%0,%1\njump %a\n1:\nadd %0,%0,%1 #}\n" 5
stmt: NEI4(reg,zero) "move %0,%0 #{ if %0 goto %a \njump 1f, eq\njump %a\n1:           #}\n"   3
stmt: NEU4(reg,zero) "move %0,%0 #{ if %0 goto %a \njump 1f, eq\njump %a\n1:           #}\n"   3
stmt: EQF4(reg,reg)  ".error \x22 floating point not supported\x22 \n"  LBURG_MAX
stmt: LEF4(reg,reg)  ".error \x22 floating point not supported\x22 \n"  LBURG_MAX
stmt: LTF4(reg,reg)  ".error \x22 floating point not supported\x22 \n"  LBURG_MAX
stmt: GEF4(reg,reg)  ".error \x22 floating point not supported\x22 \n"  LBURG_MAX
stmt: GTF4(reg,reg)  ".error \x22 floating point not supported\x22 \n"  LBURG_MAX
stmt: NEF4(reg,reg)  ".error \x22 floating point not supported\x22 \n"  LBURG_MAX
ar:   ADDRGP4     "%a"
ar:   ADDRLP4     "%a"
reg:  CALLF4(ar)  ".error \x22 floating point not supported\x22 \n"  1
reg:  CALLI4(ar)  "# emit2\n"  1
reg:  CALLP4(ar)  "# emit2\n"  1
reg:  CALLU4(ar)  "# emit2\n"  1
stmt: CALLV(ar)   "# emit2\n"  1
ar: reg    "%0"
ar: CNSTP4  "%a"   
stmt: RETF4(reg)  ".error \x22 floating point not supported\x22 \n"  1
stmt: RETI4(reg)  "# ret\n"  1
stmt: RETU4(reg)  "# ret\n"  1
stmt: RETP4(reg)  "# ret\n"  1
stmt: RETV(reg)   "# ret\n"  1
stmt: ARGF4(reg)  ".error \x22 floating point not supported\x22 \n"  LBURG_MAX
stmt: ARGI4(reg)  ".error \x22 function call arguments not yet implemented\x22 \n"  LBURG_MAX
stmt: ARGP4(reg)  ".error \x22 function call arguments not yet implemented\x22 \n"  LBURG_MAX
stmt: ARGU4(reg)  ".error \x22 function call arguments not yet implemented\x22 \n"  LBURG_MAX
stmt: ARGI4(con)  "# emit2\n"  
stmt: ARGP4(con)  "# emit2\n"  
stmt: ARGU4(con)  "# emit2\n"  
stmt: ASGNB(reg, INDIRB(reg))  "stage_rst #{ memcpy(%0, %1, %a)\nstage_inc %a \n1: \nld r3, %1, 0 \nst r3, %0, 0 \nadd %1, %1, 1 \nadd %0, %0, 1 \nstage_dec 4 \njumps 1b, 0, gt #}\n"  8

%%
static void progend(void){
	segment(CODE);
	print("halt\n");
}

static void progbeg(int argc, char *argv[]) {
        int i;

        {
                union {
                        char c;
                        int i;
                } u;
                u.i = 0;
                u.c = 1;
                swap = ((int)(u.i == 1)) != IR->little_endian;
        }

        parseflags(argc, argv);

        for (i = 0; i < 3; i++)
                ireg[i]  = mkreg("r%d", i, /*mask*/1, /*set*/IREG);
        iregw  = mkwildcard(ireg);
        tmask[IREG] = TMASKIREG;
        vmask[IREG] = VMASKIREG;
		tmask[FREG] = 0;
		vmask[FREG] = 0;
}

static Symbol rmap(int opk) {
        switch (optype(opk)) {
        case I: case U: case P: case B:
                return iregw;
        case F:
                return 0;
        default:
                return 0;
        }
}

static void target(Node p) {
    assert(p);

    switch (specific(p->op)) {
	case CALL+I: case CALL+P: case CALL+U:
		/* Even intrinsic functions return their value in R0 */
        setreg(p, ireg[0]);
        break;

    case RET+I: case RET+U: case RET+P:
        rtarget(p, 0, ireg[0]);
        break;
    }
}
static void clobber(Node p) {
        assert(p);
        switch (specific(p->op)) {

        case CALL+I: case CALL+P: case CALL+U:
            //spill(TMASKIREG, IREG, p);
            break;
        case CALL+V:
            //spill(TMASKIREG | RMASKIREG, IREG, p);
            break;
        }
}

/* Taken from gen.c */
static Node reuse(Node p, int nt) {
	struct _state {
		short cost[1];
	};
	Symbol r = p->syms[RX];

	if (generic(p->op) == INDIR && p->kids[0]->op == VREG+P
	&& r->u.t.cse && p->x.mayrecalc
	&& ((struct _state*)r->u.t.cse->x.state)->cost[nt] == 0)
		return r->u.t.cse;
	else
		return p;
}

/* Taken from gen.c */
static int getrule(Node p, int nt) {
	int rulenum;

	assert(p);
	rulenum = (*_rule)(p->x.state, nt);
	if (!rulenum) {
		fprint(stderr, "(%x->op=%s at %w is corrupt.)\n", p, opname(p->op), &src);
		assert(0);
	}
	return rulenum;
}

char* emitasmtostring(Node p, int nt) {
	int rulenum;
	short *nts;
	char *fmt;
	Node kids[10];
	char rgch[100];
	char *pch = rgch;

	p = reuse(p, nt);
	rulenum = getrule(p, nt);
	nts = _nts[rulenum];
	fmt = _templates[rulenum];
	assert(fmt);
	if (_isinstruction[rulenum] && p->x.emitted)
		return p->syms[RX]->x.name;
	else if (*fmt == '#') {
		assert(0);
	}
	else {
		if (*fmt == '?') {
			fmt++;
			assert(p->kids[0]);
			if (p->syms[RX] == p->x.kids[0]->syms[RX])
				while (*fmt++ != '\n')
					;
		}
		for ((*_kids)(p, rulenum, kids); *fmt; fmt++)
			if (*fmt != '%')
				*pch++ = *fmt;
			else if (*++fmt == 'F')
				pch += sprintf(pch, "%d", framesize);
			else if (*fmt >= '0' && *fmt <= '9') {
				strcpy (pch, emitasmtostring(kids[*fmt - '0'], nts[*fmt - '0']));
				while (*pch++ != 0)
					;
			}
			else if (*fmt >= 'a' && *fmt < 'a' + NELEMS(p->syms))
				return p->syms[*fmt - 'a']->x.name;
			else
				*pch++ = *fmt;
		*pch = 0;
	}
	return string(rgch);
}


static void emit2(Node p) {
	int i, j;
	char * fnname;
	static int argindex;
	static char* argstring[6];

	struct { const char * name; int addR0; int argcount; }
	 instr[] = {
		{ "adc",   1, 2 },
		{ "halt",  0, 0 },
		{ "i2c_rd",0, 4 },
		{ "i2c_wr",0, 5 },
		{ "reg_rd",0, 3 },
		{ "reg_wr",0, 4 },
		{ "sleep", 0, 1 },
		{ "tsens", 1, 1 },
		{ "wait",  0, 1 },
		{ "wake",  0, 0 }
	};

	switch (specific(p->op)) {
		case CALL+I: case CALL+P: case CALL+U: case CALL+V:
			fnname = p->kids[0]->syms[0]->x.name;
			for (i=0; i < NELEMS(instr); i++) {
				if (0 == strcmp(instr[i].name, fnname)) {
					print("%s ", instr[i].name);
					if (instr[i].addR0)
						print("r0,");
					for (j=0; j < instr[i].argcount; j++) {
						print(argstring[j]);
						if (j < instr[i].argcount -1)
							print(",");
					}
					print("\n");
				}
			}
			argindex = 0;
			break;

		case ARG+I: case ARG+P: case ARG+U:
		{
			int rulenum;

			/* Emulate what emitasm() does for "%0" */
			rulenum = _rule(p->x.state, /*nt*/ p->x.inst);
			argstring[argindex++] = emitasmtostring(p->kids[0], _nts[rulenum][0]);

			break;
		}
	}
}

static void doarg(Node p) {
}

static void local(Symbol p) {
	if (askregvar(p, rmap(ttob(p->type))) == 0) {
		/* Declare a new local (although our implementation is quite global) */
		int oldseg = cseg;		
		assert(p->sclass == AUTO);

		if (oldseg != BSS)
			segment(BSS);

		p->x.name = stringf("%s.%s", currentfunction, p->name);
		global(p);
		space(p->type->size);

		if (oldseg != BSS)
			segment(oldseg);
	}

}

static void function(Symbol f, Symbol caller[], Symbol callee[], int ncalls) {
        usedmask[IREG] = 0;
        freemask[IREG] = ~(unsigned)0;

		currentfunction = string(f->x.name);
        gencode(caller, callee);
        segment(CODE);
        print("%s:\n", f->x.name);
        emitcode();
		print("\n");
}

static void defconst(int suffix, int size, Value v) {
        if (suffix == F) {
                print(".error \"floating point constants not supported\n");
        }
        else if (suffix == P)
                print(".long 0x%x\n", (unsigned)v.p);
        else if (size == 1)
                print(".long 0x%x\n", (unsigned)((unsigned char)(suffix == I ? v.i : v.u)));
        else if (size == 2)
                print(".long 0x%x\n", (unsigned)((unsigned short)(suffix == I ? v.i : v.u)));
        else if (size == 4)
                print(".long 0x%x\n", (unsigned)(suffix == I ? v.i : v.u));
}

static void defaddress(Symbol p) {
                print(".long %s\n", p->x.name);
}

static void defstring(int n, char *str) {
    char *s;

    for (s = str; s < str + n; s++)
            print(".byte %d\n", (*s)&0377);
}

static void export(Symbol p) {
    print("    .global %s\n", p->x.name);
}

static void import(Symbol p) {
    if (!isfunc(p->type))
            print(".error \"importing symbols not supported\n");
}

static void defsymbol(Symbol p) {
    if (p->scope >= LOCAL && p->sclass == STATIC)
            p->x.name = stringf("L.%d", genlabel(1));
    else if (p->generated)
            p->x.name = stringf("L.%s", p->name);
    else
            assert(p->scope != CONSTANTS || isint(p->type) || isptr(p->type)),
            p->x.name = p->name;
}

static void address(Symbol q, Symbol p, long n) {
	if (isarray(p->type))
		/* We multiply n by 4 because the ULP assembler divides addresses by 4 */
		n *= 4;

	if (p->scope == GLOBAL
       || p->sclass == STATIC || p->sclass == EXTERN) {
        q->x.name = stringf("%s%s%D", p->x.name, n >= 0 ? "+" : "", n);					
	} else {
        assert(n <= INT_MAX && n >= INT_MIN);
        q->x.offset = p->x.offset + n;
        q->x.name = stringf("%s%s%D", p->x.name, n >= 0 ? "+" : "", q->x.offset);
    }
}

static void global(Symbol p) {
    print("%s:\n", p->x.name);
}
static void segment(int n) {
	int oldcseg = cseg;
	cseg = n;
	if (cseg != oldcseg) {
		switch (cseg) {
			case CODE: print(".text\n"); break;
			case LIT:  print(".text\n"); break;
			case BSS:  print(".bss\n");  break;
			case DATA: print(".data\n"); break;
		}
	}
}

static void space(int n) {
	print("    .space %d\n", roundup(n, 4));
}

static void blkloop(int dreg, int doff, int sreg, int soff, int size, int tmps[]) {
	print (".error \x22 blkloop not implemnted yet\x22 \n");
}
static void blkfetch(int size, int off, int reg, int tmp) {
	print (".error \x22 blkfetch not implemnted yet\x22 \n");
}
static void blkstore(int size, int off, int reg, int tmp) {
	print (".error \x22 blkstore not implemnted yet\x22 \n");

}
static void stabinit(char *, int, char *[]);
static void stabline(Coordinate *);
static void stabsym(Symbol);

static char *currentfile;

static int bitcount(unsigned mask) {
        unsigned i, n = 0;

        for (i = 1; i; i <<= 1)
                if (mask&i)
                        n++;
        return n;
}

/* stabinit - initialize stab output */
static void stabinit(char *file, int argc, char *argv[]) {
        if (file) {
                print(".file 2,\"%s\"\n", file);
                currentfile = file;
        }
}

/* stabline - emit stab entry for source coordinate *cp */
static void stabline(Coordinate *cp) {
        if (cp->file && cp->file != currentfile) {
                print(".file 2,\"%s\"\n", cp->file);
                currentfile = cp->file;
        }
        print(".loc 2,%d\n", cp->y);
}

/* stabsym - output a stab entry for symbol p */
static void stabsym(Symbol p) {
        if (p == cfunc && IR->stabline)
                (*IR->stabline)(&p->src);
}
Interface ulpIR = {
/* size, align, outofline */
        4, 4, 0,  /* char */
        4, 4, 0,  /* short */
        4, 4, 0,  /* int */
        4, 4, 0,  /* long */
        4, 4, 0,  /* long long */
        4, 4, 0,  /* float */
        4, 4, 0,  /* double */
        4, 4, 0,  /* long double */
        4, 4, 0,  /* pointer * */
        0, 4, 0,  /* struct */
        1,      /* little_endian */
        0,  /* mulops_calls */
        0,  /* wants_callb */
        0,  /* wants_argb */
        1,  /* left_to_right */
        0/*1 for CALL?*/,  /* wants_dag */
        0,  /* unsigned_char */
		1,  /* ptrarith_32bit */  /* ULP has weird pointer semantics: The unit is 32-bits not 8-bits */
        address,
        blockbeg,
        blockend,
        defaddress,
        defconst,
        defstring,
        defsymbol,
        emit,
        export,
        function,
        gen,
        global,
        import,
        local,
        progbeg,
        progend,
        segment,
        space,
        0, 0, 0, stabinit, stabline, stabsym, 0,
        {
                4,      /* max_unaligned_load */
                rmap,
                blkfetch, blkstore, blkloop,
                _label,
                _rule,
                _nts,
                _kids,
                _string,
                _templates,
                _isinstruction,
                _ntname,
                emit2,
                doarg,
                target,
                clobber,

        }
};
