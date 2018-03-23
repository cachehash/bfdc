#ifndef OPTIMIZE_H
#define OPTIMIZE_H

extern void optimize(Node* root, int optLevel);
extern void callAll(Node*, int (*)(Node*));
extern int clipBranch(Node**);

#endif
