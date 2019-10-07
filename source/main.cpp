#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

int main(int argc, char* argv[])
{
    consoleInit(NULL);
	hiddbgInitialize();
	hidsysInitialize();
	
	u64 UniquePadIds[2] = {};
	size_t total_entries;
	Result rc = hidsysGetUniquePadsFromNpad(CONTROLLER_HANDHELD, UniquePadIds, 2,&total_entries);
	if(R_SUCCEEDED(rc))
	{
		//Print left Joycon serial
		char serial[0x19] = {};
		hidsysGetUniquePadSerialNumber(UniquePadIds[0], serial);
		printf("Left joycon serial number: %s\n", serial);
		//Print right Joycon serial
		hidsysGetUniquePadSerialNumber(UniquePadIds[1], serial);
		printf("Right joycon serial number: %s\n", serial);
		//Colour is in BGR format
		rc = hiddbgUpdateControllerColor(0xE6B90A,0x1E0A0A,UniquePadIds[0]); //Set left Joycon to Neon Blue
		rc = hiddbgUpdateControllerColor(0x283cff,0x1E0A0A,UniquePadIds[1]); //Set left Joycon to Neon Red
		//Output if success
		if(R_SUCCEEDED(rc)) printf("Managed to set colors\n");
		else printf("Failed to set colors\n");
	}
	else printf("Something broke\n"); //Failed to get joycons

	printf("Press + to exit\n");

    // Main loop
    while (appletMainLoop())
    {
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // hidKeysDown returns information about which buttons have been
        // just pressed in this frame compared to the previous one
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }

    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
	hiddbgExit();
	hidsysExit();
    return 0;
}
