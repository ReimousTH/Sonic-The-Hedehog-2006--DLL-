#include "ArcRemover.h"

namespace ArcRemover{


	int __declspec( naked ) MainAsmHookIline(DWORD *a1, int* HandleManager, std::string* a3, std::string *a4, int* a5, int a6){
		__asm{
			mflr r12
				std       r29, -0x20(r1)
				std       r30, -0x18(r1)
				std       r31, -0x10(r1)
				stw       r12, -0x8(r1)
				stwu      r1, -0xC0(r1)
				mr        r29, r3
				lis r11,0x8258  
				ori r11,r11,0x2658 
				mtctr r11
				bctr r11
		}
	}	
	bool TOGLE = false;


	HOOK(int,__fastcall,sub_82582648,0x82582648,DWORD *a1, int* HandleManager, std::string* a3, std::string *a4, int* a5, int ShowErrors){


		DWORD test;
		__asm{
			mflr r12;
			stw r12,test
		}

		//if (TOGLE){


		//	ShowXenonMessage(L"MSG",buffer);


		// Sleep(1000);
		//}

		//std::string* po = new std::string();
		//po->append(a3->c_str());
		//po->append(" : ");
		//po->append(a4->c_str());


		//std::stringstream ss;
		//std::stringstream ss1;

		//if (a5 == 0){
		//	ss << a5;
		//}
		//else{
		//	ss << *a5;
		//}


		//ss1 << ShowErrors;



		//	po->append(" : ");
		//	po->append(ss.str().c_str());

		//	po->append(" : ");
		//	po->append(ss1.str().c_str());






		if (a3->find("shader.pkg") != std::string::npos)
		{
			//	ShowXenonMessage(L"MSG",po->c_str());
			//	Sleep(1000);
			//	return MainAsmHookIline(a1,HandleManager,a3,a4,a5,0);



		}


		return MainAsmHookIline(a1,HandleManager,a3,a4,a5,ShowErrors);



	};


	int __fastcall sub_825BE438(int a1, std::string* a2)
	{



		int res = 0;
		{

			HANDLE hFile = CreateFile( a2->c_str(), GENERIC_READ, 0, NULL, 
				OPEN_EXISTING, 0, NULL );

			if( INVALID_HANDLE_VALUE != hFile ){

				res = 1;

			}
			else{

				//	char buffer[1024];
				//	char* path = "xenon_root";
				//	sprintf(buffer,"%s",a2->c_str());
				//	ShowXenonMessage(L"MSG",buffer);
				res = 0;
			}
			CloseHandle(hFile);

		}
		return res;
	}


	void GlobalInstall()
	{


		WRITE_DWORD(0x828B30FC,0x48000010);
		INSTALL_HOOK(sub_82582648);
		WRITE_DWORD(0x8204839C,sub_825BE438);


	}


}