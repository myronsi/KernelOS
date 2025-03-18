[bits 16]

switch_to_pm:
	cli						; Отключаем прерывания (cli = clear interrupts)
	
	lgdt [gdt_descriptor]	; Загружаем GDT дескриптор (lgdt = load GDT)

	mov eax, cr0			; Чтобы перейти в PM, нужно чтобы первый бит
	or eax, 0x1				; регистра управления cr0 был 1
	mov cr0, eax

	jmp CODE_SEG:init_pm	; Делаем "дальний прыжок" в наш новый 32-битный
							; сегмент кода. Это так же заставляет процессор
							; завершить обрабатываемые в конвейере инструкции. 

[bits 32]

init_pm:					; в PM, наши старые сегменты бесполезны, поэтому
	mov ax, DATA_SEG		; мы делаем так, чтобы регистры всех сегментов
	mov ds, ax				; указывали на сегмент данных, который мы определили
	mov ss, ax				; в GDT (см. ./gdt.asm)
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ebp, 0x90000		; Обновляем позицию стека, чтобы он был на самом
	mov esp, ebp			; верху свободного места

	call BEGIN_PM			; Вызываем функцию из ./main.asm