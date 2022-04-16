#pragma once
#include <Windows.h>
#include <fstream>

typedef unsigned int(WINAPI* _LoadLibrary)(wchar_t* dllName);
typedef unsigned int(WINAPI* _GetModuleHandleA)(char* modName);

typedef int  (WINAPI* _VirtualProtect)(LPVOID lpAddress,
	SIZE_T dwSize,
	DWORD flNewProtect,
	PDWORD lpflOldProtect);




typedef struct RemoteData {
	wchar_t dllName[0xFF];  //dll·��
	unsigned  entryPoint; //��ڵ�
	unsigned hookFunction; //ע�뷽����ַ
	_LoadLibrary f_loadLibrary; //loadlibrary��ַ
	char oldCode[5];  //�洢��ڵ�
	_GetModuleHandleA f_GetModuleHandleA;
	_VirtualProtect f_VirtualProtect;

}*PRemoteData;

class Inject
{
private:
	CString game_exe_file;
public:
	BOOL startProcess(CString game_exe_file, CString game_start_arg, CString game_path, PROCESS_INFORMATION* proInfo,bool isPause = true);
	void* imageLoad(const wchar_t* filename);
	void unLoadImage(void* _data);
	DWORD getEntryPoint(const wchar_t* filename);
	//void _stdcall injectCode();
	BOOL createRemoteData(HANDLE hProcess,const wchar_t* gameExe,const wchar_t* dllName);
	void codeRemoteData(PRemoteData _data,const wchar_t* dllName);
};

