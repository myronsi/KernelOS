CC = gcc
ASM = /usr/bin/nasm
LD = ld

CFLAGS = -m32 -nostdlib -nostdinc -ffreestanding -Wall -Wextra -Iinclude
ASMFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T link.ld

# Destinations
OBJS = arch/x86/boot/multiboot.o arch/x86/boot/start.o \
       arch/x86/gdt.o arch/x86/isr.o \
       src/kernel.o src/pmm.o src/vmm.o src/isr.o src/irq.o src/vga.o src/keyboard.o \
       src/lib/string.o src/lib/stdio.o src/syscall.o

all: kernel.iso

kernel.iso: kernel.bin
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o kernel.iso isodir

kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o arch/*.o arch/x86/boot/*.o src/*.o src/lib/*.o isodir kernel.bin kernel.iso