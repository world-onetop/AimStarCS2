#include "Offsets.h"
#include "Cheats.h"
#include <iostream>
#include <stdio.h>
#include <iomanip>

int main()
{

	std::printf(
	"AimStar External for CS2\n"
	"Author: CowNow\n"
	"Source Code: https://github.com/CowNowK/AimStarCS2\n"
	"\n"
	"- Aimbot和RCS功能可能导致账号封禁，谨慎使用\n"
	"- 按下[INS]键打开或隐藏菜单\n"
	"- Aimbot and RCS features may result in VAC Bans, use with caution.\n"
	"- Press [INS] to show or hide Menu.\n"
	"\n"
	);

	auto ProcessStatus = ProcessMgr.Attach("cs2.exe");
	switch (ProcessStatus) {
	case 1:
		std::cout << "[ERROR] Please launch the game first!" << std::endl;
		goto END;
	case 2: 
		std::cout << "[ERROR] Failed to hook process, please run the cheat in UAC." << std::endl; 
		goto END; 
	case 3:
		std::cout << "[ERROR] Failed to get module address." << std::endl; 
		goto END; 
	default: 
		break;
	}

	if (!Offset::UpdateOffsets())
	{
		std::cout << "[ERROR] Failed to update offsets." << std::endl;
		goto END;
	}

	if (!gGame.InitAddress())
	{
		std::cout << "[ERROR] Failed to call InitAddress()."<< std::endl;
		goto END;
	}
	std::cout << "[Game] Process ID: " << ProcessMgr.ProcessID << std::endl;
	std::cout << "[Game] Client Address: " << gGame.GetClientDLLAddress() << std::endl;

	/*
	std::cout << "Offset:" << std::endl;
	std::cout << "--EntityList:" << std::setiosflags(std::ios::uppercase) << std::hex << Offset::EntityList << std::endl;
	std::cout << "--Matrix:" << std::setiosflags(std::ios::uppercase) << std::hex << Offset::Matrix << std::endl;
	std::cout << "--LocalPlayerController:" << std::setiosflags(std::ios::uppercase) << std::hex << Offset::LocalPlayerController << std::endl;
	std::cout << "--ViewAngles:" << std::setiosflags(std::ios::uppercase) << std::hex << Offset::ViewAngle << std::endl;
	std::cout << "--LocalPlayerPawn:" << std::setiosflags(std::ios::uppercase) << std::hex << Offset::LocalPlayerPawn << std::endl;
	*/

	std::cout << std::endl;
	std::cout << "Cheat running successfully!" << std::endl;
	std::cout << "Have fun..." << std::endl;

	try
	{
		Gui.AttachAnotherWindow("Counter-Strike 2", "SDL_app", Cheats::Run);
	}
	catch (OSImGui::OSException& e)
	{
		try
		{
			Gui.AttachAnotherWindow("反恐精英：全球攻势", "SDL_app", Cheats::Run);
		}
		catch (OSImGui::OSException& e)
		{
			std::cout << e.what() << std::endl;
		}
	}

END:
	std::cout << std::endl;
	system("pause");
	return 0;
}