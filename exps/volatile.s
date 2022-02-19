	.file	"volatile.c"
	.text
	.p2align 4
	.globl	foo
	.type	foo, @function
foo:
.LFB0:
	.cfi_startproc
	endbr64
	movl	j(%rip), %edx
	movl	i(%rip), %eax
	testl	%edx, %edx
	movl	%eax, a(%rip)
	leal	15(%rdx), %eax
	cmovns	%edx, %eax
	sarl	$4, %eax
	movl	%eax, b(%rip)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE0:
	.size	foo, .-foo
	.p2align 4
	.globl	foo1
	.type	foo1, @function
foo1:
.LFB1:
	.cfi_startproc
	endbr64
	movl	i(%rip), %eax
	movl	%eax, a(%rip)
	movl	j(%rip), %edx
	testl	%edx, %edx
	leal	15(%rdx), %eax
	cmovns	%edx, %eax
	sarl	$4, %eax
	movl	%eax, b(%rip)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE1:
	.size	foo1, .-foo1
	.p2align 4
	.globl	foo2
	.type	foo2, @function
foo2:
.LFB2:
	.cfi_startproc
	endbr64
	movl	i(%rip), %eax
	movl	j(%rip), %edx
	movl	%eax, a(%rip)
	testl	%edx, %edx
	leal	15(%rdx), %eax
	cmovns	%edx, %eax
	sarl	$4, %eax
	movl	%eax, b(%rip)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE2:
	.size	foo2, .-foo2
	.p2align 4
	.globl	foo3
	.type	foo3, @function
foo3:
.LFB3:
	.cfi_startproc
	endbr64
	movl	i(%rip), %eax
	movl	$0, b(%rip)
	movl	%eax, a(%rip)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE3:
	.size	foo3, .-foo3
	.p2align 4
	.globl	foo4
	.type	foo4, @function
foo4:
.LFB4:
	.cfi_startproc
	endbr64
	movl	i(%rip), %eax
	movl	%eax, a(%rip)
	xorl	%eax, %eax
	movl	$0, b(%rip)
	ret
	.cfi_endproc
.LFE4:
	.size	foo4, .-foo4
	.comm	j,4,4
	.comm	i,4,4
	.comm	b,4,4
	.comm	a,4,4
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
