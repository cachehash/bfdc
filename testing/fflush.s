	.file	1 "fflush.c"
	.section .mdebug.abi64
	.previous
	.nan	legacy
	.module	fp=64
	.module	oddspreg
	.abicalls
	.text
	.section	.text.startup,"ax",@progbits
	.align	2
	.align	3
	.globl	main
	.set	nomips16
	.set	nomicromips
	.ent	main
	.type	main, @function
main:
	.frame	$sp,32,$ra		# vars= 0, regs= 3/0, args= 0, gp= 0
	.mask	0x90010000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	daddiu	$sp,$sp,-32
	sd	$gp,16($sp)
	lui	$gp,%hi(%neg(%gp_rel(main)))
	daddu	$gp,$gp,$t9
	daddiu	$gp,$gp,%lo(%neg(%gp_rel(main)))
	sd	$s0,8($sp)
	ld	$s0,%got_disp(stdout)($gp)
	ld	$t9,%call16(_IO_putc)($gp)
	ld	$a1,0($s0)
	sd	$ra,24($sp)
	.reloc	1f,R_MIPS_JALR,_IO_putc
1:	jalr	$t9
	li	$a0,97			# 0x61

	ld	$t9,%call16(fflush)($gp)
	.reloc	1f,R_MIPS_JALR,fflush
1:	jalr	$t9
	ld	$a0,0($s0)

	ld	$ra,24($sp)
	ld	$gp,16($sp)
	ld	$s0,8($sp)
	move	$v0,$0
	jr	$30
	daddiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	main
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.3.0-12ubuntu1) 7.3.0"
