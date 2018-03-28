	.file	1 "char.c"
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
	.ent	getc
	.type	getc, @function
getc:
	lui	$gp,%hi(%neg(%gp_rel(getc)))
	daddu	$gp,$gp,$t9
	daddiu	$gp,$gp,%lo(%neg(%gp_rel(getc)))
	ld	$v0,%got_disp(stdin)($gp)
	ld	$t9,%call16(_IO_getc)($gp)
	ld	$a0,0($v0)
	.reloc	1f,R_MIPS_JALR,_IO_getc
1:	jr	$t9
	
	.end	getc


	.ent	putc
	.type	putc @function
putc:	
	lui	$gp,%hi(%neg(%gp_rel(putc)))
	daddu	$gp,$gp,$t9
	daddiu	$gp,$gp,%lo(%neg(%gp_rel(putc)))
	ld	$v0,%got_disp(stdout)($gp)
	ld	$t9,%call16(_IO_putc)($gp)
	ld	$a1,0($v0)
	.reloc	1f,R_MIPS_JALR,_IO_putc
1:	jr	$t9

	.end	putc


	.ent	main
	.type	main, @function
main:
	.frame	$sp,16,$ra		# vars= 0, regs= 2/0, args= 0, gp= 0
	.mask	0x90000000,-8
	.fmask	0x00000000,0
	daddiu	$sp,$sp,-16
	sd	$gp,0($sp)
	sd	$ra,8($sp)
	jal	getc
	move	$a0, $v0
	jal	putc


	ld	$ra,8($sp)
	ld	$gp,0($sp)
	move	$v0,$0
	daddiu	$sp,$sp,16
	jr	$ra

	.end	main
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.3.0-12ubuntu1) 7.3.0"
