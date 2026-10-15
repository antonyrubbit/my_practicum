	.file	"task1.c"
	.text
	.globl	has_distinct_digits
	.def	has_distinct_digits;	.scl	2;	.type	32;	.endef
	.seh_proc	has_distinct_digits
has_distinct_digits:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	pxor	%xmm0, %xmm0
	movups	%xmm0, -48(%rbp)
	movups	%xmm0, -32(%rbp)
	movq	%xmm0, -16(%rbp)
	jmp	.L2
.L5:
	movl	16(%rbp), %edx
	movslq	%edx, %rax
	imulq	$1717986919, %rax, %rax
	shrq	$32, %rax
	movl	%eax, %ecx
	sarl	$2, %ecx
	movl	%edx, %eax
	sarl	$31, %eax
	subl	%eax, %ecx
	movl	%ecx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	%eax, %eax
	subl	%eax, %edx
	movl	%edx, -4(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	testl	%eax, %eax
	je	.L3
	movl	$0, %eax
	jmp	.L6
.L3:
	movl	-4(%rbp), %eax
	cltq
	movl	$1, -48(%rbp,%rax,4)
	movl	16(%rbp), %eax
	movslq	%eax, %rdx
	imulq	$1717986919, %rdx, %rdx
	shrq	$32, %rdx
	movl	%edx, %ecx
	sarl	$2, %ecx
	cltd
	movl	%ecx, %eax
	subl	%edx, %eax
	movl	%eax, 16(%rbp)
.L2:
	cmpl	$0, 16(%rbp)
	jg	.L5
	movl	$1, %eax
.L6:
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC0:
	.ascii "%d \0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	call	__main
	movl	$1000, -4(%rbp)
	jmp	.L8
.L10:
	movl	-4(%rbp), %eax
	movl	%eax, %ecx
	call	has_distinct_digits
	testl	%eax, %eax
	je	.L9
	movl	-4(%rbp), %eax
	leaq	.LC0(%rip), %rcx
	movl	%eax, %edx
	call	printf
.L9:
	addl	$1, -4(%rbp)
.L8:
	cmpl	$9999, -4(%rbp)
	jle	.L10
	movl	$10, %ecx
	call	putchar
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev11, Built by MSYS2 project) 15.2.0"
	.def	printf;	.scl	2;	.type	32;	.endef
	.def	putchar;	.scl	2;	.type	32;	.endef
