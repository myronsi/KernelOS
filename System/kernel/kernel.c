#include "../common.h"
#include "../drivers/screen.h"


s32		kmain()
{	
	clear_screen();
		
	kprint(
		"Kernel has been successfully booted\n"
	);
	return 0;
}
