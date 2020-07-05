	.text
.LC0:
	.string	"%d\n"
printint:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	ret

	.text
	.globl	test
	.type	test, @function
test:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$25, %r8
	movq	%r8, %rdi
	movq	%rax, %r9
    ret
L1:
	popq %rbp
	ret
	.comm	x,8,8
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$5, %r8
	movq	%r8, %rdi
	call	test
	movq	%rax, %r10
	movq	%r10, x(%rip)
L2:
	popq %rbp
	ret
