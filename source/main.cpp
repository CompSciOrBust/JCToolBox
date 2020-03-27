#include <stdio.h>
#include <switch.h>
#include <string>
#include <fstream>
#include <vector>

struct configEntry
{
	std::string name;
	char leftBodyR = 0;
	char leftBodyG = 0;
	char leftBodyB = 0;
	char leftButtonR = 0;
	char leftButtonG = 0;
	char leftButtonB = 0;
	char rightBodyR = 0;
	char rightBodyG = 0;
	char rightBodyB = 0;
	char rightButtonR = 0;
	char rightButtonG = 0;
	char rightButtonB = 0;
};

std::vector <configEntry> loadConfigEntries(void);

int main()
{
	//init
	consoleInit(NULL);
	hiddbgInitialize();
	hidsysInitialize();
	bool isRunning = true;
	std::vector <configEntry> configVec = loadConfigEntries();
	int entryIndex = 0;
	//Main loop
	while(isRunning)
	{
		//Scan input
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		if (kDown & KEY_PLUS) break;
		if (kDown & KEY_DUP) entryIndex++;
		if (kDown & KEY_DDOWN) entryIndex--;
		if (entryIndex == -1) entryIndex = configVec.size() - 1;
		if (entryIndex == configVec.size()) entryIndex = 0;
		//A pressed
		if(kDown & KEY_A)
		{
			//All colour values need to be in BGR format
			configEntry ce = configVec.at(entryIndex); //I would have been your daddy but a dog beat me over the fence
			int leftBody = ce.leftBodyR + (ce.leftBodyG << 8) + (ce.leftBodyB << 16);
			printf("\x1b[4;1HSet left body: %#08X\n", leftBody);
			int leftButton = ce.leftButtonR + (ce.leftButtonG << 8) + (ce.leftButtonB << 16);
			printf("\x1b[5;1HSet left button: %#08X\n", leftButton);
			int rightBody = ce.rightBodyR + (ce.rightBodyG << 8) + (ce.rightBodyB << 16);
			printf("\x1b[6;1HSet right body: %#08X\n", rightBody);
			int rightButton = ce.rightButtonR + (ce.rightButtonG << 8) + (ce.rightButtonB << 16);
			printf("\x1b[7;1HSet right button: %#08X\n", rightButton);

			//Get joycon info
			u64 UniquePadIds[2] = {};
			s32 total_entries;
			Result rc = hidsysGetUniquePadsFromNpad(CONTROLLER_HANDHELD, UniquePadIds, 2,&total_entries);
			//If able to get info then set colours
			if(R_SUCCEEDED(rc))
			{
				//Actually apply the colours to the joycons
				rc = hiddbgUpdateControllerColor(leftBody,leftButton,UniquePadIds[0]); //Set left Joycon colours
				if(R_SUCCEEDED(rc)) printf("\x1b[8;1HSet left joycon colours successfully\n");
				else printf("\x1b[8;1HFailed to set left joycon colours\n");
				rc = hiddbgUpdateControllerColor(rightBody,rightButton,UniquePadIds[1]); //Set right joycon colours
				if(R_SUCCEEDED(rc)) printf("\x1b[9;1HSet right joycon colours successfully\n");
				else printf("\x1b[9;1HFailed to set right joycon colours\n");
			}
			else //Failed
			{
				printf("\x1b[8;1HFailed to get joycons. Are they docked?\n");
			}
			
		}
		//Print the text
		printf("\x1b[1;1HJCToolBox 0.2 by CompSciOrBust\n");
		printf("\x1b[2;1HPress + to exit, Up / Down to change colours, A to apply\n");
		printf("\x1b[3;1H\x1b[2KSelected colour set: %s\n", configVec.at(entryIndex).name.c_str());
		consoleUpdate(NULL);
	}
	//clean up
	consoleExit(NULL);
	hiddbgExit();
	hidsysExit();
}

std::vector <configEntry> loadConfigEntries()
{
	std::vector <configEntry> configVector = std::vector <configEntry>();
	std::string entryLine;
	std::ifstream configInput("sdmc:/config/JCTB.cfg");
	while (true)
	{
		if(configInput.eof()) break;
		configEntry ce = configEntry(); //Wort wort wort
		//Get the entry name
		std::getline(configInput, entryLine);
		ce.name = entryLine;
		//Get the left body rgb
		std::getline(configInput, entryLine);
		int RGB = stoi(entryLine, nullptr, 16);
		ce.leftBodyB = RGB & 0x0000ff;
		RGB = RGB >> 8;
		ce.leftBodyG = RGB & 0x0000ff;
		RGB = RGB >> 8;
		ce.leftBodyR = RGB & 0x0000ff;
		//Get the left button rgb
		std::getline(configInput, entryLine);
		RGB = stoi(entryLine, nullptr, 16);
		ce.leftButtonB = RGB & 0x0000ff;
		RGB = RGB >> 8;
		ce.leftButtonG = RGB & 0x0000ff;
		RGB = RGB >> 8;
		ce.leftButtonR = RGB & 0x0000ff;
		//Get the left body rgb
		std::getline(configInput, entryLine);
		RGB = stoi(entryLine, nullptr, 16);
		ce.rightBodyB = RGB & 0x0000ff;
		RGB = RGB >> 8;
		ce.rightBodyG = RGB & 0x0000ff;
		RGB = RGB >> 8;
		ce.rightBodyR = RGB & 0x0000ff;
		//Get the left button rgb
		std::getline(configInput, entryLine);
		RGB = stoi(entryLine, nullptr, 16);
		ce.rightButtonB = RGB & 0x0000ff;
		RGB = RGB >> 8;
		ce.rightButtonG = RGB & 0x0000ff;
		RGB = RGB >> 8;
		ce.rightButtonR = RGB & 0x0000ff;
		//Add the entry to the vecotr
		configVector.push_back(ce);
	}
	return configVector;
}