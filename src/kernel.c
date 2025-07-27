#include "../include/kb.h"
#include "../include/isr.h"
#include "../include/idt.h"
#include "../include/util.h"
#include "../include/shell.h"
#include "../include/fs.h"
void kmain(void)
{
	isr_install();
	init_fs();
	clearScreen();
	print("Hi and Welcome to KernelOS operating system\nPlease enter a command\nUse help to show available commands\n");
    launch_shell(0);    
}
