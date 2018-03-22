#include <stdio.h>

int bar() {
	return getchar();
}
int foo(char * m, int* i) {
	m[*i] = bar();
}
int main() {
	char m[30000];
	int i[1];
	foo(m, i);
}
