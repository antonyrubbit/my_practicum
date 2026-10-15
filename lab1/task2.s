	.file	"task2.c"
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
	movslq	%eax, %rdx
	imulq	$680390859, %rdx, %rdx
	shrq	$32, %rdx
	sarl	$4, %edx
	movl	%eax, %ecx
	sarl	$31, %ecx
	subl	%ecx, %edx
	imull	$101, %edx, %ecx
	subl	%ecx, %eax
	movl	%eax, %edx
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rcx
	movq	16(%rbp), %rax
	addq	%rcx, %rax
	subl	$50, %edx
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
	.globl	remove_multiples
	.def	remove_multiples;	.scl	2;	.type	32;	.endef
	.seh_proc	remove_multiples
remove_multiples:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$16, %rsp
	.seh_stackalloc	16
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	%edx, 24(%rbp)
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L8
.L10:
	movl	-8(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %ecx
	movslq	%ecx, %rax
	imulq	$1431655766, %rax, %rax
	shrq	$32, %rax
	movq	%rax, %rdx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	subl	%eax, %ecx
	movl	%ecx, %edx
	testl	%edx, %edx
	je	.L9
	movl	-8(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %ecx
	movslq	%ecx, %rax
	imulq	$1717986919, %rax, %rax
	shrq	$32, %rax
	movl	%eax, %edx
	sarl	%edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	subl	%eax, %ecx
	movl	%ecx, %edx
	testl	%edx, %edx
	je	.L9
	movl	-8(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	16(%rbp), %rax
	leaq	(%rdx,%rax), %rcx
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	cltq
	leaq	0(,%rax,4), %rdx
	movq	16(%rbp), %rax
	addq	%rax, %rdx
	movl	(%rcx), %eax
	movl	%eax, (%rdx)
.L9:
	addl	$1, -8(%rbp)
.L8:
	movl	-8(%rbp), %eax
	cmpl	24(%rbp), %eax
	jl	.L10
	movl	-4(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC1:
	.ascii "\320\230\321\201\321\205\320\276\320\264\320\275\321\213\320\271 \320\274\320\260\321\201\321\201\320\270\320\262 B:\0"
	.align 8
.LC2:
	.ascii "\320\237\320\276\321\201\320\273\320\265 \321\203\320\264\320\260\320\273\320\265\320\275\320\270\321\217 \320\272\321\200\320\260\321\202\320\275\321\213\321\205 3 \320\270\320\273\320\270 5:\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$240, %rsp
	.seh_stackalloc	240
	.seh_endprologue
	call	__main
	movl	$0, %ecx
	call	_time64
	movl	%eax, %ecx
	call	srand
	leaq	-208(%rbp), %rax
	movl	$50, %edx
	movq	%rax, %rcx
	call	fill_random
	leaq	.LC1(%rip), %rax
	movq	%rax, %rcx
	call	puts
	leaq	-208(%rbp), %rax
	movl	$50, %edx
	movq	%rax, %rcx
	call	print_array
	leaq	-208(%rbp), %rax
	movl	$50, %edx
	movq	%rax, %rcx
	call	remove_multiples
	movl	%eax, -4(%rbp)
	leaq	.LC2(%rip), %rax
	movq	%rax, %rcx
	call	puts
	movl	-4(%rbp), %edx
	leaq	-208(%rbp), %rax
	movq	%rax, %rcx
	call	print_array
	movl	$0, %eax
	addq	$240, %rsp
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
