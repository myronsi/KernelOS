#include "common.h"

void	memcpy(u8 *src, u8 *dest, u32 bytes)
{
	u32 i;

	i = 0;
	while (i < bytes)
	{
		dest[i] = src[i];
		i++;
	}
}