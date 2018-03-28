	.file	1 "tst.c"
	.section .mdebug.abi64
	.previous
	.nan	legacy
	.module	fp=64
	.module	oddspreg
	.abicalls
	.text
	.align	2
	.globl	main
	.set	nomips16
	.set	nomicromips
	.ent	main
	.type	main, @function
main:
	.frame	$fp,4112,$31		# vars= 4096, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	daddiu	$sp,$sp,-4112
	sd	$fp,4104($sp)
	move	$fp,$sp
	li	$2,8			# 0x8
	sb	$2,800($fp)
	move	$2,$0
	move	$sp,$fp
	ld	$fp,4104($sp)
	daddiu	$sp,$sp,4112
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	main
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.3.0-12ubuntu1) 7.3.0"
