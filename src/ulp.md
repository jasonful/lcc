%{

#define INTTMP 0x0000000F
#define INTVAR 0x00000000
#define INTRET 0x00000001

#define FLTTMP 0x000f0ff0
#define FLTVAR 0xfff00000

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

//static Symbol quo, rem;

%}
%start stmt

%term CNSTF2=2065
%term CNSTI2=2069
%term CNSTP2=2071
%term CNSTU2=2070

%term ARGB=41
%term ARGF2=2081
%term ARGI2=2085
%term ARGP2=2087
%term ARGU2=2086

%term ASGNB=57
%term ASGNF2=2097
%term ASGNI2=2101
%term ASGNP2=2103
%term ASGNU2=2102

%term INDIRB=73
%term INDIRF2=2113
%term INDIRI2=2117
%term INDIRP2=2119
%term INDIRU2=2118

%term CVFF2=2161
%term CVFI2=2165

%term CVIF2=2177
%term CVII2=2181
%term CVIU2=2182

%term CVPU2=2198

%term CVUI2=2229
%term CVUP2=2231
%term CVUU2=2230

%term NEGF2=2241
%term NEGI2=2245

%term CALLB=217
%term CALLF2=2257
%term CALLI2=2261
%term CALLP2=2263
%term CALLU2=2262
%term CALLV=216

%term LOADI2=2277
%term LOADU2=2278

%term RETF2=2289
%term RETI2=2293
%term RETP2=2295
%term RETU2=2294
%term RETV=248

%term ADDRGP2=2311

%term ADDRFP2=2327

%term ADDRLP2=2343

%term ADDF2=2353
%term ADDI2=2357
%term ADDP2=2359
%term ADDU2=2358

%term SUBF2=2369
%term SUBI2=2373
%term SUBP2=2375
%term SUBU2=2374

%term LSHI2=2389
%term LSHU2=2390

%term MODI2=2405
%term MODU2=2406

%term RSHI2=2421
%term RSHU2=2422

%term BANDI2=2437
%term BANDU2=2438

%term BCOMI2=2453
%term BCOMU2=2454

%term BORI2=2469
%term BORU2=2470

%term BXORI2=2485
%term BXORU2=2486

%term DIVF2=2497
%term DIVI2=2501
%term DIVU2=2502

%term MULF2=2513
%term MULI2=2517
%term MULU2=2518

%term EQF2=2529
%term EQI2=2533
%term EQU2=2534

%term GEF2=2545
%term GEI2=2549
%term GEU2=2550

%term GTF2=2561
%term GTI2=2565
%term GTU2=2566

%term LEF2=2577
%term LEI2=2581
%term LEU2=2582

%term LTF2=2593
%term LTI2=2597
%term LTU2=2598

%term NEF2=2609
%term NEI2=2613
%term NEU2=2614

%term JUMPV=584
%term LABELV=600
%term VREGP=711

%%

reg:  INDIRI2(VREGP)     "# read register\n"
reg:  INDIRU2(VREGP)     "# read register\n"

stmt: ASGNI2(VREGP,reg)  "# write register\n"
stmt: ASGNU2(VREGP,reg)  "# write register\n"

con: CNSTI2  "%a"
con: CNSTU2  "%a"

stmt: reg  ""

acon: con     "%0"
acon: ADDRGP2 "%a"

addr: ADDI2(reg,con)  "%0, %1"
addr: ADDU2(reg,con)  "%0, %1"
addr: ADDP2(reg,con)  "%0, %1"
addr: acon  "%0"
reg: addr  "move %c,%0\n"  1
stmt: ASGNI2(reg,reg)  "st %1,%0,0\n"  1
stmt: ASGNU2(reg,reg)  "st %1,%0,0\n"  1
reg:  INDIRI2(reg)     "ld %c,%0,0\n"  1
reg:  INDIRU2(reg)     "ld %c,%0,0\n"  1

reg:  INDIRF2(addr)     ".error \x22float not supported\x22\n"  1
stmt: ASGNF2(addr,reg)  ".error \x22float not supported\x22\n"  1
reg: DIVI2(reg,reg)  ".error \x22Division not supported.  Try right shift >>\x22\n"   1
reg: DIVU2(reg,reg)  ".error \x22Division not supported.  Try right shift >>\x22\n"  1
reg: MODI2(reg,reg)  ".error \x22Mod not supported\x22\n"   1
reg: MODU2(reg,reg)  ".error \x22Mod not supported\x22\n"  1
reg: MULI2(reg,reg)  ".error \x22Multiplication not supported. Try left shift <<\x22\n"   1
reg: MULU2(reg,reg)  ".error \x22Multiplication not supported. Try left shift <<\x22\n"   1
rc:  con            "%0"
rc:  reg            "%0"

reg: ADDI2(reg,rc)   "add %c,%0,%1\n"  1
reg: ADDP2(reg,rc)   "add %c,%0,%1\n"  1
reg: ADDU2(reg,rc)   "add %c,%0,%1\n"  1
reg: BANDI2(reg,rc)  "and %c,%0,%1\n"  1
reg: BORI2(reg,rc)   "or  %c,%0,%1\n"    1
reg: BXORI2(reg,rc)  ".error \x22xor not supported\x22\n"   1
reg: BANDU2(reg,rc)  "and %c,%0,%1\n"   1
reg: BORU2(reg,rc)   "or %c,%0,%1\n"    1
reg: BXORU2(reg,rc)  ".error \x22xor not supported\x22\n"   1
reg: SUBI2(reg,rc)   "sub %c,%0,%1\n"  1
reg: SUBP2(reg,rc)   "sub %c,%0,%1\n"  1
reg: SUBU2(reg,rc)   "sub %c,%0,%1\n"  1
rc16bit: CNSTI2         "%a"  range(a,0,0xFFFF)
rc16bit: reg            "%0"

reg: LSHI2(reg,rc16bit)  "lsh %c,%0,%1\n"  1
reg: LSHU2(reg,rc16bit)  "lsh %c,%0,%1\n"  1
reg: RSHI2(reg,rc16bit)  "rsh %c,%0,%1\n"  1
reg: RSHU2(reg,rc16bit)  "rsh %c,%0,%1\n"  1
reg: BCOMI2(reg)  "move %c, 0\nsub %c,%c,%0\nsub %c, %c, 1\n"   1
reg: BCOMU2(reg)  "move %c, 0\nsub %c,%c,%0\nsub %c, %c, 1\n"   1
reg: NEGI2(reg)   "move %c, 0\nsub %c,%c,%0\n"  1
reg: LOADI2(reg)  "move %c,%0\n"  move(a)
reg: LOADU2(reg)  "move %c,%0\n"  move(a)
reg: ADDF2(reg,reg)  ".error \x22floating point addition not supported\x22\n"  1
reg: DIVF2(reg,reg)  ".error \x22floating point division not supported\x22\n"  1
reg: MULF2(reg,reg)  ".error \x22floating point multiplication not supported\x22\n"  1
reg: SUBF2(reg,reg)  ".error \x22floating point subtraction not supported\x22\n"  1
reg: NEGF2(reg)      ".error \x22floating point negation not supported\x22\n"       1
reg: CVII2(reg)  "move %c, %0\n"  2
reg: CVUI2(reg)  "move %c, %0\n"  1
reg: CVUU2(reg)  "move %c, %0\n"  1
reg: CVFF2(reg)  ".error \x22floating point not supported\x22\n"  1
reg: CVIF2(reg)  ".error \x22floating point not supported\x22\n"  2
reg: CVFI2(reg)  ".error \x22floating point not supported\x22\n" 
stmt: LABELV  "%a:\n"
stmt: JUMPV(acon)  "jump %0\n"   1
stmt: JUMPV(reg)   "jump %0\n" 
stmt: EQI2(reg,reg)  "sub %0,%0,%1\njump %a, eq\n"   1
stmt: EQU2(reg,reg)  "sub %0,%0,%1\njump %a, eq\n"   1
stmt: GEI2(reg,reg)  "sub %0,%1,%0\njump %a, eq\njump %a, ov\n"   1
stmt: GEU2(reg,reg)  "sub %0,%1,%0\njump %a, eq\njump %a, ov\n"  1
stmt: GTI2(reg,reg)  "sub %0,%1,%0\njump %a, ov\n"   1
stmt: GTU2(reg,reg)  "sub %0,%1,%0\njump %a, ov\n"  1
stmt: LEI2(reg,reg)  "sub %0,%0,%1\njump %a, eq\njump %a, ov\n"   1
stmt: LEU2(reg,reg)  "sub %0,%0,%1\njump %a, eq\njump %a, ov\n"  1
stmt: LTI2(reg,reg)  "sub %0,%0,%1\njump %a, ov\n"   1
stmt: LTU2(reg,reg)  "sub %0,%0,%1\njump %a, ov\n"  1
stmt: NEI2(reg,reg)  "sub %0,%0,%1\njump %L, eq\njump %a\n%L:\n"   1
stmt: NEU2(reg,reg)  "sub %0,%0,%1\njump %L, eq\njump %a\n%L:\n"   1
stmt: EQF2(reg,reg)  ".error \x22floating point not supported\x22\n"  2
stmt: LEF2(reg,reg)  ".error \x22floating point not supported\x22\n"  2
stmt: LTF2(reg,reg)  ".error \x22floating point not supported\x22\n"  2
stmt: GEF2(reg,reg)  ".error \x22floating point not supported\x22\n"  2
stmt: GTF2(reg,reg)  ".error \x22floating point not supported\x22\n"  2
stmt: NEF2(reg,reg)  ".error \x22floating point not supported\x22\n"  2
ar:   ADDRGP2     "%a"

reg:  CALLF2(ar)  ".error \x22floating point not supported\x22\n"  1
reg:  CALLI2(ar)  ".error \x22function calls not yet implemented\x22\n"  1
reg:  CALLP2(ar)  ".error \x22function calls not yet implemented\x22\n"  1
reg:  CALLU2(ar)  ".error \x22function calls not yet implemented\x22\n"  1
stmt: CALLV(ar)   ".error \x22function calls not yet implemented\x22\n"  1
ar: reg    "%0"
ar: CNSTP2  "%a"   
stmt: RETF2(reg)  ".error \x22floating point not supported\x22\n"  1
stmt: RETI2(reg)  "# ret\n"  1
stmt: RETU2(reg)  "# ret\n"  1
stmt: RETP2(reg)  "# ret\n"  1
stmt: RETV(reg)   "# ret\n"  1
stmt: ARGF2(reg)  ".error \x22floating point not supported\x22\n"  1
stmt: ARGI2(reg)  ".error \x22function call arguments not yet implemented\x22\n"  1
stmt: ARGP2(reg)  ".error \x22function call arguments not yet implemented\x22\n"  1
stmt: ARGU2(reg)  ".error \x22function call arguments not yet implemented\x22\n"  1

%%
static void progend(void){}

static void progbeg(int argc, char *argv[]) {
        int i;
		int useheaders = 1;

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
        for (i = 0; i < argc; i++)
            if (strncmp(argv[i], "-n", 2) == 0) {
				useheaders = 0;
			}

		if (useheaders) {
			print("#include \"soc/rtc_cntl_reg.h\"\n");
			print("#include \"soc/rtc_io_reg.h\"\n");
			print("#include \"soc/soc_ulp.h\"\n");
		}
        for (i = 0; i < 4; i++)
                ireg[i]  = mkreg("R%d", i, /*mask*/1, /*set*/IREG);
        iregw  = mkwildcard(ireg);
        tmask[IREG] = INTTMP; 
        vmask[IREG] = INTVAR; 
		tmask[FREG] = FLTTMP;
		vmask[FREG] = FLTVAR;
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
 
        case RET+I: case RET+U: case RET+P:
                rtarget(p, 0, ireg[0]);
                break;
        }
}
static void clobber(Node p) {
        assert(p);
        switch (specific(p->op)) {

        case CALL+I: case CALL+P: case CALL+U:
                spill(INTTMP,          IREG, p);
                break;
        case CALL+V:
                spill(INTTMP | INTRET, IREG, p);
                break;
        }
}
static void emit2(Node p) {
       
}

static void doarg(Node p) {
       
}

static void local(Symbol p) {
        if (askregvar(p, rmap(ttob(p->type))) == 0)
                mkauto(p);
}

static void function(Symbol f, Symbol caller[], Symbol callee[], int ncalls) {
        usedmask[IREG] = 0;
        freemask[IREG] = ~(unsigned)0;

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
        print(".global %s\n", p->x.name);
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
        if (p->scope == GLOBAL
        || p->sclass == STATIC || p->sclass == EXTERN)
                q->x.name = stringf("%s%s%D", p->x.name,
                        n >= 0 ? "+" : "", n);
        else {
                assert(n <= INT_MAX && n >= INT_MIN);
                q->x.offset = p->x.offset + n;
                q->x.name = stringd(q->x.offset);
        }
}

static void global(Symbol p) {
        print("/* global */\n%s:\n", p->x.name);
}
static void segment(int cseg) {
    switch (cseg) {
		case CODE: print(".text\n"); break;
		case LIT:  print(".text\n"); break;
		case BSS:  print(".bss\n");  break;
		case DATA: print(".data\n"); break;
    }
}
static void space(int n) {
        if (cseg != BSS) {
				int extra = n % 4;
				if (extra)
					n += (4 - extra);
                print(".space %d\n", n);
		}
}
static void blkloop(int dreg, int doff, int sreg, int soff, int size, int tmps[]) {
        int lab = genlabel(1);

        print("addu $%d,$%d,%d\n", sreg, sreg, size&~7);
        print("addu $%d,$%d,%d\n", tmps[2], dreg, size&~7);
        blkcopy(tmps[2], doff, sreg, soff, size&7, tmps);
        print("L.%d:\n", lab);
        print("addu $%d,$%d,%d\n", sreg, sreg, -8);
        print("addu $%d,$%d,%d\n", tmps[2], tmps[2], -8);
        blkcopy(tmps[2], doff, sreg, soff, 8, tmps);
        print("bltu $%d,$%d,L.%d\n", dreg, tmps[2], lab);
}
static void blkfetch(int size, int off, int reg, int tmp) {
        assert(size == 1 || size == 2 || size == 4);
        if (size == 1)
                print("lbu $%d,%d($%d)\n",  tmp, off, reg);
        else if (salign >= size && size == 2)
                print("lhu $%d,%d($%d)\n",  tmp, off, reg);
        else if (salign >= size)
                print("lw $%d,%d($%d)\n",   tmp, off, reg);
        else if (size == 2)
                print("ulhu $%d,%d($%d)\n", tmp, off, reg);
        else
                print("ulw $%d,%d($%d)\n",  tmp, off, reg);
}
static void blkstore(int size, int off, int reg, int tmp) {
        if (size == 1)
                print("sb $%d,%d($%d)\n",  tmp, off, reg);
        else if (dalign >= size && size == 2)
                print("sh $%d,%d($%d)\n",  tmp, off, reg);
        else if (dalign >= size)
                print("sw $%d,%d($%d)\n",  tmp, off, reg);
        else if (size == 2)
                print("ush $%d,%d($%d)\n", tmp, off, reg);
        else
                print("usw $%d,%d($%d)\n", tmp, off, reg);
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
        2, 2, 0,  /* char */
        2, 2, 0,  /* short */
        2, 2, 0,  /* int */
        2, 2, 0,  /* long */
        2, 2, 0,  /* long long */
        2, 2, 0,  /* float */
        2, 2, 0,  /* double */
        2, 2, 0,  /* long double */
        2, 2, 0,  /* pointer * */
        0, 2, 0,  /* struct */
        1,      /* little_endian */
        0,  /* mulops_calls */
        0,  /* wants_callb */
        0,  /* wants_argb */
        1,  /* left_to_right */
        0/*1 for CALL?*/,  /* wants_dag */
        0,  /* unsigned_char */
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
static char rcsid[] = "$Id$";
