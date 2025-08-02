#include "../include/kb.h"
#include "../include/isr.h"
#include "../include/idt.h"
#include "../include/util.h"
#include "../include/shell.h"
#include "../include/fs.h"

void kmain(void)
{
    clearScreen();
    print("Starting kmain\n");
    asm("cli");
    print("Calling isr_install\n");
    isr_install();
    print("ISR installed\n");
    print("Calling init_fs\n");
    init_fs();
    print("File system initialized\n");
    print("\n");
    print("Welcome to KernelOS\nPlease enter a command\nUse help to show available commands\n");
    print("\n");
    asm("sti");
    print("Calling launch_shell\n");
    launch_shell(0);
}