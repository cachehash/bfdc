#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "hash/hash.h"

int callAll(Node* n, int(*f)(Node**)) {
	int changed = 0;
	if (n == NULL) {
		return 0;
	}
	switch (n->type) {
	case LOOP:
		return f(&n->n[0].n);
	break;
	case STMTS:
		changed |= f(&n->n[0].n);
		changed |= f(&n->n[1].n);
		return changed;
	break;
	case SUM:
	case SHIFT:
	case OUT:
	case IN:
	case SET:
		return 0;
	break;
	}
	return 0;
}
int clipBranch(Node**);
/*
 * concatenate multiple adds/subs together
 */
int join(Node** np) {
	Node *n = *np;
	if (n == NULL) {
		return 0;
	}
	int changed = 0;
	if (n->type == STMTS) {
		Node* lchild = n->n[0].n;
		Node* rtmp = n;
		while (1) {
			//get next STMTS
			rtmp = rtmp->n[1].n;
			if (rtmp == NULL) {
				break;
			}
			//get next STMT from code
			Node* rlchild = rtmp->n[0].n;
			//concatenate if same type of SUMs and SHIFTs
			if (lchild->type == rlchild->type && (lchild->type == SUM || lchild->type == SHIFT)) {
				lchild->n[0].i += rlchild->n[0].i;
				//remove the STMT and STMTS from tree and free them
				n->n[1].n = rtmp->n[1].n;
				free(rlchild);
				free(rtmp);
				changed = 1;
			} else {
				break;
			}
		}
	}
	changed |= callAll(n, join);
	return changed;
}
int nullify(Node** np) {
	Node* n = *np;
	if (n == NULL) {
		return 0;
	}
	int changed = 0;
	Node* left = n->n[0].n;
	if (n->type == STMTS) {
		//eliminate +- and ><
		if (((left->type == SUM || left->type == SHIFT) && left->n[0].i == 0)) {
			//remove n from tree and free it
			*np = n->n[1].n;
			free(n);
			changed = 1;
		}
	}
	changed |= callAll(n, nullify);
	return changed;
}
int useSet(Node** np) {
	Node *n = *np;
	if (n == NULL) {
		return 0;
	}
	int changed = 0;
	if (n->type == LOOP) {
		if (n->n[0].n == NULL) {
			*np = NULL;
			return 1;
		}
		Node* body = n->n[0].n;
		if (body->type != STMTS) {
			goto ret;
		}
		/*
		 * perform static analysis to see if it's only composed of 
		 * +-<> and always lands on the same cell.
		 */
		int netMove = 0;
		for (Node* node = body; node != NULL; node = node->n[1].n) {
			Node* left = node->n[0].n;
			if (!(left && (left->type == SHIFT || left->type == SUM))) {
				goto ret;
			}
			if (left->type == SHIFT) {
				netMove += left->n[0].i;
			}
		}
		if (netMove == 0) {
			/*
			 * the cell the loop starts on/ends on must be zero to exit the loop.
			 * This means we can set it to 0 and add it/subtract it to the other
			 * cells.
			 */
			Map* m = newIntPtrMap(100);
			/*
			 * guarenty that if someone does [>+<] we don't
			 * assume wrongly that zero is in the map
			 */
			int* zero = calloc(1, sizeof(int));
			mPut(m, zero, zero);
			for (Node* node = body; node != NULL; node = node->n[1].n) {
				Node* left = node->n[0].n;
				if (left->type == SHIFT) {
					netMove += left->n[0].i;
				} else {
					int* n = mGet(m, &netMove);
					if (n == NULL) {
						n = calloc(1, sizeof(int));
						mPut(m, &netMove, n);
					}
					*n += left->n[0].i;
				}
			}
			void* keys[m->size];
			mGetKeys(m, keys);
			/*
			 * first prune the map of redundant values
			 */
			for (int i = 0; i < m->size; i++) {
				int* k = keys[i];
				int* val = mGet(m, k);
				if (*k != 0 && *val == 0) {
					mDel(m, k);
				}
			}
			mGetKeys(m, keys);
			int quantity = m->size-1;
			Node* new = mkNode(quantity, SET);
			int newIndx = 0;
			for (int i = 0; i < m->size; i++) {
				int* k = keys[i];
				int* val = mGet(m, k);
				if (*k == 0) {
					//TODO warn if loop is infinite
				} else {
					Point *p = &new->n[newIndx].p;
					p->x = *k;
					p->y = *val;
					newIndx++;
				}
			}
			destroyMap(m);
			clipBranch(np);
			*np = new;
			n = NULL;
		}

	}
ret:
	changed |= callAll(*np, useSet);
	return changed;
}
void optimize(Node* n) {
	int changed = 1;
	while (changed) {
		changed = 0;
		changed |= join(&n);
		changed |= nullify(&n);
		if (!changed) {
			changed |= useSet(&n);
		}
	}
}
int clipBranch(Node** np) {
	Node *n = *np;
	if (n == NULL) {
		return 0;
	}
	callAll(n, clipBranch);
	free(n);
	return 0;
}
