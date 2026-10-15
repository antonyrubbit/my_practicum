	.file	"task1.c"
	.text
	.globl	has_distinct_digits
	.def	has_distinct_digits;	.scl	2;	.type	32;	.endef
	.seh_proc	has_distinct_digits
has_distinct_digits:
	subq	$56, %rsp
	.seh_stackalloc	56
	.seh_endprologue
	pxor	%xmm0, %xmm0
	movups	%xmm0, (%rsp)
	movups	%xmm0, 16(%rsp)
	movq	$0, 32(%rsp)
	testl	%ecx, %ecx
	jg	.L3
	movl	$1, %eax
	jmp	.L1
.L6:
	movl	%eax, %ecx
.L3:
	movslq	%ecx, %rax
	imulq	$1717986919, %rax, %rax
	sarq	$34, %rax
	movl	%ecx, %edx
	sarl	$31, %edx
	subl	%edx, %eax
	leal	(%rax,%rax,4), %edx
	addl	%edx, %edx
	movl	%ecx, %eax
	subl	%edx, %eax
	movslq	%eax, %rdx
	cmpl	$0, (%rsp,%rdx,4)
	jne	.L5
	movl	$1, (%rsp,%rdx,4)
	movslq	%ecx, %rax
	imulq	$1717986919, %rax, %rax
	sarq	$34, %rax
	movl	%ecx, %edx
	sarl	$31, %edx
	subl	%edx, %eax
	cmpl	$9, %ecx
	jg	.L6
	movl	$1, %eax
	jmp	.L1
.L5:
	movl	$0, %eax
.L1:
	addq	$56, %rsp
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
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	call	__main
	movl	$1000, %ebx
	leaq	.LC0(%rip), %rsi
	jmp	.L9
.L8:
	addl	$1, %ebx
	cmpl	$10000, %ebx
	je	.L11
.L9:
	movl	%ebx, %ecx
	call	has_distinct_digits
	testl	%eax, %eax
	je	.L8
	movl	%ebx, %edx
	movq	%rsi, %rcx
	call	printf
	jmp	.L8
.L11:
	movl	$10, %ecx
	call	putchar
	movl	$0, %eax
	addq	$40, %rsp
	popq	%rbx
	popq	%rsi
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev11, Built by MSYS2 project) 15.2.0"
	.def	printf;	.scl	2;	.type	32;	.endef
	.def	putchar;	.scl	2;	.type	32;	.endef
