#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#ifndef CELL_T
#define CELL_T uint8_t
#endif

#ifndef NUM_CELLS
#define NUM_CELLS 30000
#endif

CELL_T readChar(int prev) {
	int x = getchar();
	return (x == EOF) ? -1 : x;
}
int main() {
	size_t i = 0;
	CELL_T m[NUM_CELLS] = {0};
	m[i] += 8;
	m[i+1] += 8*m[i];
	m[i] = 0;
	i += 1;
	m[i-1] += 4*m[i];
	m[i] = 0;
	m[i] += 1;
	i += -1;
	while(m[i]) {
		i += 1;
		m[i] += -1;
		i += -1;
		m[i+1] += 4*m[i];
		m[i] = 0;
		i += 1;
		m[i-1] += 8*m[i];
		m[i] = 0;
		i += -1;
		m[i+1] += 8*m[i];
		m[i] = 0;
		m[i] += 1;
		i += 1;
		while(m[i]) {
			i += 1;
			m[i] += 10;
			m[i+1] += 5*m[i];
			m[i] = 0;
			i += 1;
			m[i] += 1;
			putchar(m[i]);
			fflush(stdout);
			m[i] += -1;
			putchar(m[i]);
			fflush(stdout);
			m[i] = 0;
			i += -2;
			m[i] = 0;
			i += -1;
			m[i] += -1;
			i += 1;
		}
		i += -1;
		while(m[i]) {
			i += 2;
			m[i] += 7;
			m[i+1] += 7*m[i];
			m[i] = 0;
			i += 1;
			putchar(m[i]);
			fflush(stdout);
			m[i] += 5;
			putchar(m[i]);
			fflush(stdout);
			m[i] = 0;
			i += -3;
			m[i] += -1;
		}
	}
	i += 1;
	while(m[i]) {
		i += 1;
		m[i] += 8;
		m[i+1] += 7*m[i];
		m[i] = 0;
		i += 1;
		putchar(m[i]);
		fflush(stdout);
		m[i] = 0;
		i += -2;
		m[i] += -1;
	}
	i += -1;
	m[i] += 11;
	m[i+1] += 3*m[i];
	m[i+2] += 9*m[i];
	m[i+3] += 9*m[i];
	m[i+4] += m[i];
	m[i] = 0;
	i += 1;
	m[i] += -1;
	putchar(m[i]);
	fflush(stdout);
	i += 1;
	m[i] += -1;
	putchar(m[i]);
	fflush(stdout);
	m[i] += 7;
	putchar(m[i]);
	fflush(stdout);
	m[i] += 11;
	putchar(m[i]);
	fflush(stdout);
	i += -1;
	putchar(m[i]);
	fflush(stdout);
	i += 2;
	putchar(m[i]);
	fflush(stdout);
	m[i] += 2;
	putchar(m[i]);
	fflush(stdout);
	m[i] += 7;
	putchar(m[i]);
	fflush(stdout);
	putchar(m[i]);
	fflush(stdout);
	i += -1;
	m[i] += -1;
	putchar(m[i]);
	fflush(stdout);
	i += 2;
	m[i] += -1;
	putchar(m[i]);
	fflush(stdout);
	while(m[i]) {
		m[i] = 0;
		i += -1;
	}
}
