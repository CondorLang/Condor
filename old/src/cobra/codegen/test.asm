section .text
	global Add ; Declare Func Add
	global main ; Declare Func main

Add:
	push ebp ; Save EBP
	mov ebp, esp
	sub esp, 0
	mov a,[ebp + 8]
	mov b,[ebp + 16]


main: