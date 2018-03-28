#ifndef COMP_H
#define COMP_H

extern void comp();
extern int level;
extern void iprintfln(const char* fmt,...);
extern void iprintf(const char* fmt,...);
extern void iprintAdd(int x);
extern void iprintCoeff(int x);
extern void iprintDiv(int x);
extern void compC();
extern void compGo();
extern void compSpim();
extern void compMips();

#endif
