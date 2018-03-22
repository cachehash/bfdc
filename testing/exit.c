void exit(int status) {
	asm (
	"mov %0, %%ebx \n\t"
	"mov $1, %%eax \n\t"
	"syscall"
	:
	: "r" (status));
}
void _start() {
	exit(42);
}
