#include "common.h"

void display_menu(char **menu_entries, int total_entries, int *menuindex, char *headerstr)
{
	int i;
	u32 redraw = 1;
	u32 kDown = 0;

	while(1)
	{
		kDown = HID_PAD;

		if(redraw)
		{
			redraw = 0;
			console_init();
			printf("%s\n\n", headerstr);

			for(i=0; i < total_entries; i++)
			{
				if(*menuindex == i)
				{
					printf("-> ");
				}
				else
				{
					printf("   ");
				}

				printf("%s\n", menu_entries[i]);
			}
		}

		if(kDown & KEY_B)
		{
			*menuindex = -1;
			return;
		}

		if(kDown & KEY_DDOWN)
		{
			(*menuindex)++;
			
			if(*menuindex >= total_entries)
				*menuindex = 0;

			redraw = 1;

			continue;
		}
		else if(kDown & KEY_DUP)
		{
			(*menuindex)--;

			if(*menuindex < 0)
				*menuindex = total_entries-1;

			redraw = 1;

			continue;
		}

		if(kDown & KEY_A)
		{
			break;
		}
	}
	return;
}
