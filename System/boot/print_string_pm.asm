[bits 32]					; Используем 32-битный режим

							; Определяем некоторые константы
VIDEO_MEMORY equ 0xb8000	; = адрес начала памяти VGA
WHITE_ON_BLACK equ 0x0f		; = цвет символов (0x0f - белый на черном)

print_string_pm:
	pusha
	mov edx, VIDEO_MEMORY	; Перемещаем в EDX адрес начала массива видеопамяти

print_string_pm_loop:
							; Помним, что AX (2б) = AH(1б) и AL(1б)
	mov al, [ebx]			; Сохраняем символ из EBX в AL
	mov ah, WHITE_ON_BLACK	; Устанавливаем цвет символов в AH
							; таким образом AX получается равен символу + цвету

	cmp al, 0				; if (AL == 0), т.е. если конец массива, то
	je print_string_pm_done	; заканчиваем выполнение функции
							; else:
	mov [edx], ax			; video_memory[EDX] = AX
	add ebx, 1				; переходим к следующему символу (+1, просто массив)
	add edx, 2				; переходим к следующему адресу в VGA (+2 т.к.
							; два байта на символ)

	jmp print_string_pm_loop

print_string_pm_done:
	popa
	ret
