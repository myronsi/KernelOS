gdt_start:					; Эта пустая метка нужно для того чтобы удобнее
							; посчитать размер GDT для ее дескриптора 
							; (end - start)

gdt_null:					; Необходимый нулевой дескриптор для GDT
	dd 0x0					; dd - define double (двойное слово, т.е. 4 байта)
	dd 0x0

gdt_code:					; Определяем дескриптор сегмента кода
	dw 0xffff				; Limit (bits 0-15)
	dw 0x0					; Base (bits 0-15)
	db 0x0					; Base (bits 16-23)
	db 10011010b			; Первые флаги + флаги типа (смотрим по битам)
							; present: 1, privilege: 00, descriptor type: 1
							; code: 1, conforming: 0, readable: 1, accessed: 0
	db 11001111b			; Вторые флаги + длина сегмента (bits 16-19):
							; granularity: 1, 32-bit default: 1,
							; 64-bit default: 0, AVL: 0
	db 0x0					; Base (bits 24-31)

gdt_data:					; Определяем дескриптор сегмента кода
	dw 0xffff				; Limit (bits 0-15)
	dw 0x0					; Base (bits 0-15)
	db 0x0					; Base (bits 16-23)
	db 10010010b			; Первые флаги + флаги типа (смотрим по битам)
	db 11001111b			; Вторые флаги + длина сегмента (bits 16-19)
	db 0x0					; Base (bits 24-31)

gdt_end:					; Пустая метка


gdt_descriptor:						; дескриптор GDT
	dw gdt_end - gdt_start - 1		; Размер GDT
	dd gdt_start					; Адрес начала GDT


CODE_SEG equ gdt_code - gdt_start	; Определяем некоторые константы. 
DATA_SEG equ gdt_data - gdt_start	; Они понадобятся для регистров сегментов в
									; 32-битном защищенном режиме. Например,
									; когда мы установим регистр DS = 0x10 (т.е
									; 16 байтов) в этом режиме, процессор
									; поймет что мы хотим использовать сегмент,
									; находящийся в смещении 0x10 в нашем GDT,
									; т.е. в нашем случае это сегмент данных
									; (0x0 -> NULL, 0x08 -> сегмент кода,
									; 0x10 -> сегмент данных)