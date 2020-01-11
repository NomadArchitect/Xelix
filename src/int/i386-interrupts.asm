; interrupts.asm: Hardware part of interrupt handling
; Copyright © 2010-2019 Lukas Martini

; This file is part of Xelix.
;
; Xelix is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; Xelix is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with Xelix.  If not, see <http://www.gnu.org/licenses/>.

[EXTERN cpu_fault_handler]
[EXTERN interrupts_callback]
[EXTERN vmem_kernel_hwdata]

%define PIT_MASTER	0x20
%define PIT_SLAVE	0xA0
%define PIT_CONFIRM	0x20
%define IRQ7		39
%define IRQ15		47

%macro INTERRUPT 1
	[GLOBAL interrupts_handler%1]
	interrupts_handler%1:
		; Push dummy value for exception error code field
		push dword 0
		push esp

		; Need to store user esp in the struct, so account for the added field
		add dword [esp], 4
		pusha
		mov ebx, %1
		jmp interrupts_common_handler
%endmacro

%macro INTERRUPT_ERRCODE 1
	[GLOBAL interrupts_handler%1]
	interrupts_handler%1:
		; Error code is implicitly pushed by CPU before the exception handler
		push esp
		add dword [esp], 4
		pusha
		mov ebx, %1
		jmp interrupts_common_handler
%endmacro

%assign i 0
%rep 256
	%if (i == 8 || i == 17 || i == 30 || (i >= 10 && i <= 14))
		INTERRUPT_ERRCODE i
	%else
		INTERRUPT i
	%endif
	%assign i i+1
%endrep

; Handles interrupt hardware handling. Expects interrupt number in ebx. Returns
; 1 in eax if the interrupt was spurious, 0 otherwise.
handle_eoi:
	; Is this a spurious interrupt on the master PIC? If yes, return
	cmp ebx, IRQ7
	je .spurious

	; Do we have to send an EOI (End of interrupt)?
	cmp ebx, 31
	jle .return

	; Send EOI to master PIC
	mov al, PIT_CONFIRM
	out PIT_MASTER, al

	; Is this a spurious interrupt on the secondary PIC? If yes, return
	; (We do this here so the master PIC still receives an EOI as it can't
	; know about the spuriousness of this interrupt).
	cmp ebx, IRQ15
	je .spurious

	; Check if we have to send an EOI to the secondary PIC
	cmp ebx, 39
	jle .return

	; Send EOI to secondary PIC
	mov al, PIT_CONFIRM
	out PIT_SLAVE, al
.return:
	mov eax, 0
	ret
.spurious:
	mov eax, 1
	ret

; This is the common interrupt handler. It saves the processor state, sets up
; for kernel mode segments, handles spurious interrupts, calls the C-level
; handler, and finally restores the stack frame.

interrupts_common_handler:
	; We have to push all the stuff in isf_t (int/int.h) which
	; interrupts_callback takes in reversed order.
	;
	; The cpu automatically pushes eflags, cs, and eip, and an error code for
	; some exceptions. Our macros push esp, and the interrupt number. The rest
	; has to be put into the struct here.

	; push ds, cr2 & cr3
	xor eax, eax
	mov ax, ds
	push eax

	mov eax, cr2
	push eax

	mov eax, cr3
	push eax

	; load the kernel data segment descriptor
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; ebx still contains the interrupt number from the handlers above
	call handle_eoi
	test eax, eax
	jnz .return

	; Load kernel paging context from global variable set during
	; early boot in vmem_init
	mov ecx, [vmem_kernel_hwdata]
	mov cr3, ecx

	; Call C handler with fastcall convention
	mov ecx, ebx
	mov edx, esp
 	call interrupts_callback

	; interrupts_callback returns an isf_t interrupt stack frame
	mov esp, eax

.return:
	; Set paging context
	pop eax
	mov cr3, eax

	; Skip cr2
	add esp, 4

	; reload segment descriptors
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	popa
	pop esp
	iret
