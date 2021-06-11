.386


DGROUP  group   _DATA

_DATA   segment word public 'DATA'
		
;; MAGIC number to send to backdoor api
BDOOR_MAGIC	equ	564D5868H
	
;; Low-bandwidth backdoor port number 
;; for the IN/OUT interface.
BDOOR_PORT	equ	
	5658H

_DATA   ends

assume  cs:_TEXT,ds:DGROUP

_TEXT   segment word public 'CODE'
        assume  CS:_TEXT

        public  RetP_
RetP_   proc    near   ; int Ret4()
	add     EAX, 1
        ret
RetP_   endp

	public Backdoor_
Backdoor_	proc	near	;  int Backdoor(int)
		
		mov eax, 564D5868H
		mov ebx, 0
		mov ecx, 6h
		mov dx, 5658H
		in eax, dx
		nop
		add eax, 1
Backdoor_	endp
	
_TEXT   ends
        end