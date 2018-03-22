#ifndef OPTIMIZE_H
#define OPTIMIZE_H

extern void optimize();
extern void callAll(Node*, int (*)(Node*));
extern int clipBranch(Node**);

#endif
