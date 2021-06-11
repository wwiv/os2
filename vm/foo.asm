.386


DGROUP  group   _DATA

_DATA   segment word public 'DATA'
		
;; MAGIC number to send to backdoor api
BDOOR_MAGIC	equ	564D5868H
	
;; Low-bandwidth backdoor port number 
;; for the IN/OUT interface.
BDOOR_PORT	equ	5658H

_DATA   ends

assume  cs:_TEXT,ds:DGROUP

_TEXT   segment word public 'CODE'
        assume  CS:_TEXT

	;; int Backdoor(int)
	public Backdoor_
Backdoor_	proc	near
 		mov ecx, eax
		mov eax, BDOOR_MAGIC
		mov ebx, 0
		mov dx, BDOOR_PORT
		in eax, dx
		ret
Backdoor_	endp
	
_TEXT   ends
        end