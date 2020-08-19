	.text
	.global zfunction

zfunction:
	pushl	%ebp
	movl	8(%esp),%eax
	andl	$0xfffc01ff,%eax
	sarl	$8,%eax
	orl	$0xff000000,%eax
	popl	%ebp
	ret
