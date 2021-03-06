#include "data.h"
#include "decl.h"
#include "glob.h"

static int freereg[4];
static char *reglist[4] = { "%r8", "%r9", "%r10", "%r11" };
static char *breglist[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };
static char *dreglist[4] = { "r8d", "%r9d", "%r10d", "%r11d" };

static char *cmplist[] = { "sete", "setne", "setg", "setle", "setge" };
static char *invcmplist[] = { "jne", "je", "jge", "jle", "jg", "jl" };

static int psize[] = {0, 0, 1, 4, 8};

void freeall_registers(void)
{
  freereg[0]= freereg[1]= freereg[2]= freereg[3]= 1;
}

int alloc_register(void)
{
  for (int i=0; i<4; i++) {
    if (freereg[i]) {
      freereg[i]= 0;
      return(i);
    }
  }
  fprintf(stderr, "Out of registers!\n");
  exit(1);
}

void free_register(int reg)
{
  if (freereg[reg] != 0) {
    fprintf(stderr, "Error trying to free register %d\n", reg);
    exit(1);
  }
  freereg[reg]= 1;
}

void cgpreamble()
{
  freeall_registers();
  fputs("\t.text\n"
	".LC0:\n"
	"\t.string\t\"%d\\n\"\n"
	"printint:\n"
	"\tpushq\t%rbp\n"
	"\tmovq\t%rsp, %rbp\n"
	"\tsubq\t$16, %rsp\n"
	"\tmovl\t%edi, -4(%rbp)\n"
	"\tmovl\t-4(%rbp), %eax\n"
	"\tmovl\t%eax, %esi\n"
	"\tleaq	.LC0(%rip), %rdi\n"
	"\tmovl	$0, %eax\n"
	"\tcall	printf@PLT\n" "\tnop\n" "\tleave\n" "\tret\n" "\n", outp);
}

void cgpostamble()
{
  fputs(
	"\tmovl	$0, %eax\n"
	"\tpopq	%rbp\n"
	"\tret\n",
  outp);
}

void cgfuncpreamble(char *name) {
  fprintf(outp,
          "\t.text\n"
          "\t.globl\t%s\n"
          "\t.type\t%s, @function\n"
          "%s:\n" "\tpushq\t%%rbp\n"
          "\tmovq\t%%rsp, %%rbp\n", name, name, name);
}

// Print out a function postamble
void cgfuncpostamble(int id) {
    cglabel(gsym[id].endlabel);
    fprintf(outp, "\tpopq %rbp\n\tret\n");
}

int cgloadint(int value, int type) {

  // Get a new register
  int r= alloc_register();

  fprintf(outp, "\tmovq\t$%d, %s\n", value, reglist[r]);
  return(r);
}

int cgloadglob(int id) {
    int r = alloc_register();

    switch(gsym[id].type) {
        case P_U8:
            fprintf(outp, "\tmovzbq\t%s(\%%rip), %s\n", gsym[id].name,reglist[r]);
            break;
        case P_I32:
            fprintf(outp, "\tmovzbl\t%s(\%%rip), %s\n", gsym[id].name, reglist[r]);
            break;
        case P_I64:
            fprintf(outp, "\tmovq\t%s(\%%rip), %s\n", gsym[id].name, reglist[r]);
            break;
        default:
            fatald("bad type in cgloadglob:", gsym[id].type);
    }
    return r;
}


int cgstorglob(int r, int id) {
    switch(gsym[id].type) {
        case P_U8:
            fprintf(outp, "\tmovb\t%s, %s(\%%rip)\n", breglist[r], gsym[id].name);
            break;
        case P_I32:
            fprintf(outp, "\tmovl\t%s, %s(\%%rip)\n", dreglist[r], gsym[id].name);
            break;
        case P_I64:
            fprintf(outp, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], gsym[id].name);
            break;
        default:
            fatald("bad type in cgstorglob:", gsym[id].type);
    }
    return (r);
}

void cgglobsym(int id) {
    int typesize;
    typesize = cgprimsize(gsym[id].type);

    fprintf(outp, "\t.comm\t%s,%d,%d\n", gsym[id].name, typesize, typesize);
}

int cgcall(int r, int id) {
    int outr = alloc_register();
    fprintf(outp, "\tmovq\t%s, %%rdi\n", reglist[r]);
    fprintf(outp, "\tcall\t%s\n", gsym[id].name);
    fprintf(outp, "\tmovq\t%%rax, %s\n", reglist[outr]);
    free_register(r);
    return outr;
}

void cgreturn(int reg, int id) {
    switch(gsym[id].type) {
        case P_U8:
            fprintf(outp, "\tmovzbl\t%s, %%eax\n", breglist[reg]);
            break;
        case P_I32:
            fprintf(outp, "\tmovl\t%s, %%eax\n", dreglist[reg]);
            break;
        case P_I64:
            fprintf(outp, "\tmovq\t%s, %%rax\n", reglist[reg]);
            break;
        default:
            fatald("bad function type in cgreturn:", gsym[id].type);
    }
    cgjump(gsym[id].endlabel);
}

int cgwiden(int r, int oldtype, int newtype) {
    return r;
}

int cgadd(int r1, int r2) {
  fprintf(outp, "\taddq\t%s, %s\n", reglist[r1], reglist[r2]);
  free_register(r1);
  return(r2);
}

int cgsub(int r1, int r2) {
  fprintf(outp, "\tsubq\t%s, %s\n", reglist[r2], reglist[r1]);
  free_register(r2);
  return(r1);
}

int cgmul(int r1, int r2) {
  fprintf(outp, "\timulq\t%s, %s\n", reglist[r1], reglist[r2]);
  free_register(r1);
  return(r2);
}

int cgdiv(int r1, int r2) {
  fprintf(outp, "\tmovq\t%s,%%rax\n", reglist[r1]);
  fprintf(outp, "\tcqo\n");
  fprintf(outp, "\tidivq\t%s\n", reglist[r2]);
  fprintf(outp, "\tmovq\t%%rax,%s\n", reglist[r1]);
  free_register(r2);
  return(r1);
}

void cgprintint(int r) {
  fprintf(outp, "\tmovq\t%s, %%rdi\n", reglist[r]);
  fprintf(outp, "\tcall\tprintint\n");
  free_register(r);
}

int cgcompare_and_set(int ast_op, int r1, int r2) {
  if (ast_op < A_EQ || ast_op > A_GE)
    fatal("Bad ast_op in cgcompare_and_set()");

  fprintf(outp, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  fprintf(outp, "\t%s\t%s\n", cmplist[ast_op - A_EQ], breglist[r2]);
  fprintf(outp, "\tmovzbq\t%s, %s\n", breglist[r2], reglist[r2]);
  free_register(r1);
  return (r2);
}

void cglabel(int l) {
	fprintf(outp, "L%d:\n", l);
}

void cgjump(int l) {
	fprintf(outp, "\tjmp\tL%d\n", l);
}

int cgcompare_and_jump(int ast_op, int r1, int r2, int label) {
  if (ast_op < A_EQ || ast_op > A_GE)
    fatal("Bad ast_op in cgcompare_and_set()");

  fprintf(outp, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  fprintf(outp, "\t%s\tL%d\n", invcmplist[ast_op - A_EQ], label);
  freeall_registers();
  return (NOREG);
}

int cgprimsize(int type) {
    if(type < P_NONE || type > P_I64)
        fatal("bad type in cgprimsize()");
    return(psize[type]);
}
