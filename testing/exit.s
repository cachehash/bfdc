	.file	"exit.c"
	.text
	.globl	exit
	.type	exit, @function
exit:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
#APP
	mov %ebx, %eax
	mov $1, %eax
	syscall
#NO_APP
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	exit, .-exit
	.globl	_start
	.type	_start, @function
_start:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$42, %edi
	call	exit
	.cfi_endproc
.LFE1:
	.size	_start, .-_start
	.ident	"GCC: (Ubuntu 7.3.0-5ubuntu1) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
