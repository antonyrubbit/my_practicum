	.file	"task1.c"
	.text
	.p2align 4
	.globl	has_distinct_digits
	.def	has_distinct_digits;	.scl	2;	.type	32;	.endef
	.seh_proc	has_distinct_digits
has_distinct_digits:
	subq	$56, %rsp
	.seh_stackalloc	56
	.seh_endprologue
	pxor	%xmm0, %xmm0
	movq	$0, 32(%rsp)
	movups	%xmm0, (%rsp)
	movups	%xmm0, 16(%rsp)
	testl	%ecx, %ecx
	jle	.L2
	movl	$3435973837, %r9d
	jmp	.L4
	.p2align 6
	.p2align 4,,10
	.p2align 3
.L8:
	movl	$1, (%rsp,%rax,4)
	cmpl	$9, %ecx
	jle	.L2
	movl	%r8d, %ecx
.L4:
	movl	%ecx, %eax
	imulq	%r9, %rax
	shrq	$35, %rax
	leal	(%rax,%rax,4), %edx
	movl	%eax, %r8d
	movl	%ecx, %eax
	addl	%edx, %edx
	subl	%edx, %eax
	cltq
	movl	(%rsp,%rax,4), %edx
	testl	%edx, %edx
	je	.L8
	xorl	%eax, %eax
	addq	$56, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L2:
	movl	$1, %eax
	addq	$56, %rsp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC0:
	.ascii "%d \0"
	.section	.text.startup,"x"
	.p2align 4
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$104, %rsp
	.seh_stackalloc	104
	movups	%xmm6, 80(%rsp)
	.seh_savexmm	%xmm6, 80
	.seh_endprologue
	movl	$1000, %esi
	pxor	%xmm6, %xmm6
	movl	$3435973837, %ebx
	call	__main
	.p2align 4
	.p2align 3
.L13:
	movq	$0, 64(%rsp)
	movl	%esi, %edx
	movups	%xmm6, 32(%rsp)
	movups	%xmm6, 48(%rsp)
	.p2align 6
	.p2align 4
	.p2align 3
.L11:
	movl	%edx, %eax
	imulq	%rbx, %rax
	shrq	$35, %rax
	leal	(%rax,%rax,4), %ecx
	movl	%eax, %r8d
	movl	%edx, %eax
	addl	%ecx, %ecx
	subl	%ecx, %eax
	cltq
	movl	32(%rsp,%rax,4), %edx
	testl	%edx, %edx
	jne	.L14
	movl	$1, 32(%rsp,%rax,4)
	movl	%r8d, %edx
	testl	%r8d, %r8d
	jne	.L11
	movl	%esi, %edx
	leaq	.LC0(%rip), %rcx
	call	printf
.L14:
	addl	$1, %esi
	cmpl	$10000, %esi
	jne	.L13
	movl	$10, %ecx
	call	putchar
	nop
	movups	80(%rsp), %xmm6
	xorl	%eax, %eax
	addq	$104, %rsp
	popq	%rbx
	popq	%rsi
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev11, Built by MSYS2 project) 15.2.0"
	.def	printf;	.scl	2;	.type	32;	.endef
	.def	putchar;	.scl	2;	.type	32;	.endef
