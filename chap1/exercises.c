#include "programming/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tree *T_tree;
struct tree {T_tree left; string key; T_tree right;};
T_tree Tree(T_tree l, string k, T_tree r) {
	T_tree t = checked_malloc(sizeof(*t));
	t->left=l; t->key=k; t->right=r;
	return t;
}

T_tree insert(string key, T_tree t) {
	if (t==NULL) return Tree(NULL, key, NULL);
	else if (strcmp(key, t->key) < 0)
		return Tree(insert(key, t->left), t->key, t->right);
	else if (strcmp(key, t->key) > 0)
		return Tree(t->left, t->key, insert(key, t->right));
	else return Tree(t->left, key, t->right);
}

// a.
bool member(string key, T_tree t) {
	// Taking advantage of short-circuiting
	return t!=NULL && t->key==key || member(key, t->left) || member(key, t->right);
}

// b.
typedef struct treeB *T_treeB;
struct treeB {T_treeB left; string key; void *binding; T_treeB right;};
T_treeB TreeB(T_treeB l, string k, void *binding, T_treeB r) {
	T_treeB t = checked_malloc(sizeof(*t));
	t->left=l; t->key=k; t->binding=binding; t->right=r;
	return t;
}
T_treeB insertB(string key, void *binding, T_treeB t) {
	if (t==NULL) return TreeB(NULL, key, binding, NULL);
	else if (strcmp(key, t->key) < 0)
		return TreeB(insertB(key, binding, t->left), t->key, t->binding, t->right);
	else if (strcmp(key, t->key) > 0)
		return TreeB(t->left, t->key, t->binding, insertB(key, binding, t->right));
	else return TreeB(t->left, key, binding, t->right);
}

// c.
// (a)
//      t
//     s 
//    p
//   i
//  f
// b
//
// (b)
// a
//  b
//   c
//    d
//     e
//      f
//       g
//        h
//         i

// d.
// I think AVL tree and Red-Black tree fit the need

int main() {
	return EXIT_SUCCESS;
}
