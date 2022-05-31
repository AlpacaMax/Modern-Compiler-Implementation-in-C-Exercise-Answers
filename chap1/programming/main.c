#include "util.h"
#include "slp.h"
#include "prog1.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct table *Table_;
int length(A_expList);
int maxargsExp(A_exp);
int maxargs(A_stm);
Table_ Table(string, int, struct table *);
int lookup(Table_, string);
void interp(A_stm);
Table_ interpStm(A_stm, Table_);
struct IntAndTable interpExp(A_exp, Table_);

struct table {string id; int value; Table_ tail;};
struct IntAndTable {int i; Table_ t;};

int length(A_expList expList) {
	int c = 0;
	A_expList curr = expList;

	for(; curr->kind == A_pairExpList; ++c) curr = curr->u.pair.tail;

	return c+1;
}

// We need to consider EseqExp which could contain a print statement
int maxargsExp(A_exp exp) {
	int leftResult = 0;
	int rightResult = 0;

	if (exp->kind == A_eseqExp) {
		leftResult = maxargs(exp->u.eseq.stm);
		rightResult = maxargsExp(exp->u.eseq.exp);
	}

	return leftResult < rightResult ? rightResult : leftResult;
}

// Deal with ExpList which could contain a EseqExp
int maxargsExpList(A_expList expList) {
	int maxlength = 0;
	int locallength;
	A_expList curr = expList;

	while (curr->kind == A_pairExpList) {
		locallength = maxargsExp(curr->u.pair.head);
		maxlength = maxlength < locallength ? locallength : maxlength;

		curr = curr->u.pair.tail;
	}

	locallength = maxargsExp(curr->u.last);
	return maxlength < locallength ? locallength : maxlength;
}

int maxargs(A_stm stm) {
	int leftResult = 0;
	int rightResult = 0;

	if (stm->kind == A_compoundStm) {
		leftResult = maxargs(stm->u.compound.stm1);
		rightResult = maxargs(stm->u.compound.stm2);
	} else if (stm->kind == A_printStm) {
		leftResult = length(stm->u.print.exps);
		rightResult = maxargsExpList(stm->u.print.exps);
	} else if (stm->kind == A_assignStm) {
		leftResult = maxargsExp(stm->u.assign.exp);
	}

	return leftResult < rightResult ? rightResult : leftResult;
}

Table_ Table(string id, int value, struct table *tail) {
	Table_ t = malloc(sizeof(*t));
	t->id=id; t->value=value; t->tail=tail;
	return t;
}

int lookup(Table_ t, string key) {
	Table_ curr = t;

	while (curr != NULL) {
		if (curr->id == key) {
			return curr->value;
		}
		curr = curr->tail;
	}
	printf("\nError: %s not found\n", key);

	return 0;
}

void interp(A_stm stm) {
	interpStm(stm, NULL);
}

Table_ interpStm(A_stm stm, Table_ t) {
	Table_ result = t;
	struct IntAndTable resultExp;

	if (stm->kind == A_compoundStm) {
		result = interpStm(stm->u.compound.stm1, result);
		result = interpStm(stm->u.compound.stm2, result);
	} else if (stm->kind == A_assignStm) {
		resultExp = interpExp(stm->u.assign.exp, result);
		result = Table(stm->u.assign.id, resultExp.i, resultExp.t);
	} else if (stm->kind == A_printStm) {
		A_expList es = stm->u.print.exps;
		for (; es->kind != A_lastExpList; es = es->u.pair.tail) {
			resultExp = interpExp(es->u.pair.head, result);
			printf("%d ", resultExp.i);
			result = resultExp.t;
		}
		resultExp = interpExp(es->u.last, result);
		printf("%d\n", resultExp.i);
		result = resultExp.t;
	}

	return result;
}

struct IntAndTable interpExp(A_exp e, Table_ t) {
	int value;

	if (e->kind == A_idExp) {
		value = lookup(t, e->u.id);
	} else if (e->kind == A_numExp) {
		value = e->u.num;
	} else if (e->kind == A_opExp) {
		struct IntAndTable leftResult = interpExp(e->u.op.left, t);
		struct IntAndTable rightResult = interpExp(e->u.op.right, leftResult.t);
		t = rightResult.t;

		if (e->u.op.oper == A_plus) {
			value = leftResult.i + rightResult.i;
		} else if (e->u.op.oper == A_minus) {
			value = leftResult.i - rightResult.i;
		} else if (e->u.op.oper == A_times) {
			value = leftResult.i * rightResult.i;
		} else {
			value = leftResult.i / rightResult.i;
		}
	} else if (e->kind == A_eseqExp) {
		t = interpStm(e->u.eseq.stm, t);
		struct IntAndTable result = interpExp(e->u.eseq.exp, t);
		t = result.t;
		value = result.i;
	}

	return (struct IntAndTable) { value, t };
}

int main() {
	interp(prog());
	return 0;
}
