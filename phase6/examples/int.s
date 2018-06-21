main:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$main.size, %eax
	subq	%rax, %rsp
	mov	$100, %eax
	movl	%eax, -4(%rbp)
	mov	$30, %edi
	movl	%edi, -8(%rbp)
	mov	$2, %esi
	movl	%esi, -12(%rbp)
#ADD
#ADD
	mov	-4(%rbp), %edx
	add	-8(%rbp), %edx
	add	-12(%rbp), %edx
	mov	%edx, %ecx
	movl	%ecx, -16(%rbp)
#SUBTRACT
#SUBTRACT
	mov	-4(%rbp), %edx
	sub	-8(%rbp), %edx
	sub	-12(%rbp), %edx
	mov	%edx, %r8d
	movl	%r8d, -20(%rbp)
#MULTIPLY
#MULTIPLY
	mov	-4(%rbp), %edx
	imul	-8(%rbp), %edx
	imul	-12(%rbp), %edx
	mov	%edx, %r9d
	movl	%r9d, -24(%rbp)
#ADD
#DIVIDE
	mov	%eax, -64(%rbp)	# spill
	mov	-4(%rbp), %eax
	mov	%esi, -68(%rbp)	# spill
	mov	-8(%rbp), %esi
	cltd
	idivl	%esi
	add	-12(%rbp), %eax
	mov	%eax, %esi
	movl	%esi, -28(%rbp)
#SUBTRACT
#REMAINDER
	mov	-4(%rbp), %eax
	mov	%esi, -80(%rbp)	# spill
	mov	-8(%rbp), %esi
	cltd
	idivl	%esi
	sub	-12(%rbp), %edx
	mov	%edx, %esi
	movl	%esi, -32(%rbp)
#ADDRESS
	leaq	.L1, %rdx
	movq		%rdx, -92(%rbp)
	mov	%eax, -96(%rbp)	# spill
	mov	%edi, -100(%rbp)	# spill
	mov	%esi, -104(%rbp)	# spill
	mov	%ecx, -108(%rbp)	# spill
	mov	%r8d, -112(%rbp)	# spill
	mov	%r9d, -116(%rbp)	# spill
	movl	-16(%rbp), %esi
	movq	-92(%rbp), %rdi
	movl	$0, %eax
	call	printf
	movl	%eax, -120(%rbp)
#ADDRESS
	leaq	.L2, %rax
	movq		%rax, -128(%rbp)
	movl	-20(%rbp), %esi
	movq	-128(%rbp), %rdi
	movl	$0, %eax
	call	printf
	movl	%eax, -132(%rbp)
#ADDRESS
	leaq	.L3, %rax
	movq		%rax, -140(%rbp)
	movl	-24(%rbp), %esi
	movq	-140(%rbp), %rdi
	movl	$0, %eax
	call	printf
	movl	%eax, -144(%rbp)
#ADDRESS
	leaq	.L4, %rax
	movq		%rax, -152(%rbp)
	movl	-28(%rbp), %esi
	movq	-152(%rbp), %rdi
	movl	$0, %eax
	call	printf
	movl	%eax, -156(%rbp)
#ADDRESS
	leaq	.L5, %rax
	movq		%rax, -164(%rbp)
	movl	-32(%rbp), %esi
	movq	-164(%rbp), %rdi
	movl	$0, %eax
	call	printf
	movl	%eax, -168(%rbp)
.L0:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	main.size, 176
	.globl	main

.L1:	.asciz "%d\n"
.L2:	.asciz "%d\n"
.L3:	.asciz "%d\n"
.L4:	.asciz "%d\n"
.L5:	.asciz "%d\n"
