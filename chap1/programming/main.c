#include "util.h"
#include "slp.h"
#include "prog1.h"
#include <stdio.h>

int length(A_expList);
int maxargsExp(A_exp);
int maxargs(A_stm);

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

int main() {
	printf("%d\n", maxargs(prog()));
	return 0;
}
