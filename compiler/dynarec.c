#include <stdio.h>

void printOut(char* c) {
	putchar(*c);
	fflush(stdout);
}
char readIn() {
	return getchar();
}
int dynarec() {
#if defined(__mips__)
	fprintf(stderr, "ERROR: mips is not supported\n");
	int buff[] = {
		0x67bdfff8,				//daddiu $sp, $sp, -8
		0xffbf0000,				//sd $ra, ($sp)
		0x0c000000|((int) readIn)>>2,		//jal readIn
		0,					//nop
		0x00402025,				//or $a0, $v0, $0
		0x0c000000|((int) printOut)>>2,
		0,
		0xdfbf0018,				//ld $ra ($sp)
		0x67bd0008,				//daddiu $sp, $sp, 8
		0x03e00008
	};
	void (*f)() = (void*) buff;
	f();
	return 1;
#elif defined(__arm__)
	fprintf(stderr, "ERROR: arm is not supported\n");
	return 1;
#elif defined(__x86_64__)
	fprintf(stderr, "ERROR: x86_64 is not supported\n");
	return 1;
#else
	fprintf(stderr, "ERROR: your architecture is not supported\n");
	return 1;
#endif
}
