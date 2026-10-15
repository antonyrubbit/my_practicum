	.file	"task3.c"
	.text
	.globl	fill_random
	.def	fill_random;	.scl	2;	.type	32;	.endef
	.seh_proc	fill_random
fill_random:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	%edx, 24(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L2
.L3:
	call	rand
	movl	%eax, %ecx
	movslq	%ecx, %rax
	imulq	$1676084799, %rax, %rax
	shrq	$32, %rax
	movl	%eax, %edx
	sarl	$4, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$3, %eax
	addl	%edx, %eax
	subl	%eax, %ecx
	movl	%ecx, %edx
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rcx
	movq	16(%rbp), %rax
	addq	%rcx, %rax
	subl	$20, %edx
	movl	%edx, (%rax)
	addl	$1, -4(%rbp)
.L2:
	movl	-4(%rbp), %eax
	cmpl	24(%rbp), %eax
	jl	.L3
	nop
	nop
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC0:
	.ascii "%4d \0"
	.text
	.globl	print_array
	.def	print_array;	.scl	2;	.type	32;	.endef
	.seh_proc	print_array
print_array:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	%edx, 24(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L5
.L6:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	leaq	.LC0(%rip), %rcx
	movl	%eax, %edx
	call	printf
	addl	$1, -4(%rbp)
.L5:
	movl	-4(%rbp), %eax
	cmpl	24(%rbp), %eax
	jl	.L6
	movl	$10, %ecx
	call	putchar
	nop
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	find_closest
	.def	find_closest;	.scl	2;	.type	32;	.endef
	.seh_proc	find_closest
find_closest:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$16, %rsp
	.seh_stackalloc	16
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	%edx, 24(%rbp)
	movl	%r8d, 32(%rbp)
	movl	$0, -4(%rbp)
	movq	16(%rbp), %rax
	movl	(%rax), %eax
	subl	32(%rbp), %eax
	movl	%eax, %edx
	negl	%edx
	cmovns	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	$1, -12(%rbp)
	jmp	.L8
.L10:
	movl	-12(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	subl	32(%rbp), %eax
	movl	%eax, %edx
	negl	%edx
	cmovns	%edx, %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jge	.L9
	movl	-16(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -4(%rbp)
.L9:
	addl	$1, -12(%rbp)
.L8:
	movl	-12(%rbp), %eax
	cmpl	24(%rbp), %eax
	jl	.L10
	movl	-4(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC1:
	.ascii "\320\234\320\260\321\201\321\201\320\270\320\262 Q:\0"
.LC2:
	.ascii "\320\222\320\262\320\265\320\264\320\270\321\202\320\265 X: \0"
.LC3:
	.ascii "%d\0"
	.align 8
.LC4:
	.ascii "\320\247\320\270\321\201\320\273\320\276 %d \320\275\320\260\320\271\320\264\320\265\320\275\320\276.\12\0"
	.align 8
.LC5:
	.ascii "\320\247\320\270\321\201\320\273\320\276 %d \320\275\320\265 \320\275\320\260\320\271\320\264\320\265\320\275\320\276. \320\221\320\273\320\270\320\266\320\260\320\271\321\210\320\265\320\265: %d (\320\270\320\275\320\264\320\265\320\272\321\201 %d)\12\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$112, %rsp
	.seh_stackalloc	112
	.seh_endprologue
	call	__main
	movl	$0, %ecx
	call	_time64
	movl	%eax, %ecx
	call	srand
	leaq	-64(%rbp), %rax
	movl	$10, %edx
	movq	%rax, %rcx
	call	fill_random
	leaq	.LC1(%rip), %rax
	movq	%rax, %rcx
	call	puts
	leaq	-64(%rbp), %rax
	movl	$10, %edx
	movq	%rax, %rcx
	call	print_array
	leaq	.LC2(%rip), %rax
	movq	%rax, %rcx
	call	printf
	leaq	-68(%rbp), %rax
	leaq	.LC3(%rip), %rcx
	movq	%rax, %rdx
	call	scanf
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L13
.L16:
	movl	-8(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-68(%rbp), %eax
	cmpl	%eax, %edx
	jne	.L14
	movl	$1, -4(%rbp)
	jmp	.L15
.L14:
	addl	$1, -8(%rbp)
.L13:
	cmpl	$9, -8(%rbp)
	jle	.L16
.L15:
	cmpl	$0, -4(%rbp)
	je	.L17
	movl	-68(%rbp), %eax
	leaq	.LC4(%rip), %rcx
	movl	%eax, %edx
	call	printf
	jmp	.L18
.L17:
	movl	-68(%rbp), %edx
	leaq	-64(%rbp), %rax
	movl	%edx, %r8d
	movl	$10, %edx
	movq	%rax, %rcx
	call	find_closest
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-68(%rbp), %eax
	movl	-12(%rbp), %r8d
	leaq	.LC5(%rip), %rcx
	movl	%r8d, %r9d
	movl	%edx, %r8d
	movl	%eax, %edx
	call	printf
.L18:
	movl	$0, %eax
	addq	$112, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev11, Built by MSYS2 project) 15.2.0"
	.def	rand;	.scl	2;	.type	32;	.endef
	.def	printf;	.scl	2;	.type	32;	.endef
	.def	putchar;	.scl	2;	.type	32;	.endef
	.def	srand;	.scl	2;	.type	32;	.endef
	.def	puts;	.scl	2;	.type	32;	.endef
	.def	scanf;	.scl	2;	.type	32;	.endef
