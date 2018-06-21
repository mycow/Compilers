parameters size3
Offset: 104
c:
	tpushq	%rbp
	movq	%rsp, %rbp
	subq	$120, %rsp

	movq	%rbp, %rsp
	popq	%rbp
	ret

parameters size0
Offset: 0
main:
	tpushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp

	movq	%rbp, %rsp
	popq	%rbp
	ret

