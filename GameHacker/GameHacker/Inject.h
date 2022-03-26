#pragma once
#include <fstream>

typedef unsigned int(WINAPI* _LoadLibrary)(wchar_t* dllName);

typedef struct RemoteData {
	wchar_t dllName[0xFF];  //dllÂ·¾¶
	_LoadLibrary f_loadLibrary; //loadlibraryµØÖ·

}*PRemoteData;

class Inject
{
private:
	CString game_exe_file;
public:
	BOOL startProcess(CString game_exe_file, CString game_start_arg, CString game_path, PROCESS_INFORMATION* proInfo);
	void* imageLoad(wchar_t* filename);
	void unLoadImage(void* _data);
	DWORD getEntryPoint(wchar_t* filename);
	//void _stdcall injectCode();
	BOOL createRemoteData(HANDLE hProcess, wchar_t* dllName);
	void codeRemoteData(PRemoteData _data,  wchar_t* dllName);
};

