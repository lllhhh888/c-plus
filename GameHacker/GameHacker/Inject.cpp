#include "pch.h"
#include "Inject.h"


//注入函数
void _stdcall injectCode() {
	unsigned address = 0xCCCCCCCC;
	PRemoteData p = (PRemoteData)address;
	p->f_loadLibrary(p->dllName);
	unsigned dEntry = p->entryPoint; 
	char* entryCode = (char*)p->entryPoint;
	entryCode[0] = p->oldCode[0];
	entryCode[1] = p->oldCode[1];
	entryCode[2] = p->oldCode[2];
	entryCode[3] = p->oldCode[3];
	entryCode[4] = p->oldCode[4];
	_asm {
		mov eax,dEntry
		jmp eax
	}
	//LoadLibrary(L"E:\\c++\\GameHacker\\Release\\testdll.dll");
}

DWORD _stdcall RemoteThreadProc( PRemoteData p) {
	unsigned base = p->f_GetModuleHandleA(0);  //获取内存地址
	DWORD dRet;
	p->entryPoint += base;   // 400000 + 25D20
	p->f_VirtualProtect((LPVOID)p->entryPoint, 0x5, PAGE_EXECUTE_READWRITE, &dRet);
	char* entryCode = (char*)p->entryPoint;
	//保存入口点
	p->oldCode[0] = entryCode[0];
	p->oldCode[1] = entryCode[1];
	p->oldCode[2] = entryCode[2];
	p->oldCode[3] = entryCode[3];
	p->oldCode[4] = entryCode[4];
	int* entryDis = (int*)(p->entryPoint + 1);
	
	entryCode[0] = 0xE9;
	int distance = p->hookFunction - p->entryPoint - 5;

	entryDis[0] = distance;
	return 1;
}



BOOL Inject::startProcess(CString _game_exe_file, CString _game_start_arg, CString _game_path, PROCESS_INFORMATION* proInfo, bool isPause) {

	
	STARTUPINFO startUpInfo{};
	startUpInfo.cb = sizeof(startUpInfo);
    

	game_exe_file = _game_exe_file;
	DWORD dFlags = 0;
	if (isPause)dFlags = CREATE_SUSPENDED;
	//
	return  CreateProcess(
		_game_exe_file,
		_game_start_arg.GetBuffer(),
		NULL, NULL, TRUE,
		dFlags,
		NULL,
		_game_path,
		&startUpInfo,
		proInfo
	);



}

void* Inject::imageLoad(const wchar_t* filename) {
	//以二进制的形式读取
	std::ifstream steamRead(filename, std::ios::binary);
	//跳转到尾部
	steamRead.seekg(0, std::ios::end);
	//获取文件大小
	unsigned filesize = steamRead.tellg();
	//分配内存
	char* _data = new char[filesize];
	//跳转到头部
	steamRead.seekg(0, std::ios::beg);
	//读取进内存
	steamRead.read(_data, filesize);
	//关闭
	steamRead.close();
	return _data;

}

void Inject::unLoadImage(void* _data) {
	delete[] _data;
}

DWORD Inject::getEntryPoint(const wchar_t* filename) {
	void* image = imageLoad(filename);
	//获取dos头
	IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)image;
	//pe地址
	unsigned PEAddress = dosHeader->e_lfanew + (unsigned)image;
	//获取nt头
	IMAGE_NT_HEADERS* ntHeader = (IMAGE_NT_HEADERS*)PEAddress;
	//h获取入口点
	DWORD entryPoint = ntHeader->OptionalHeader.AddressOfEntryPoint;
	unLoadImage(image);
	return entryPoint;
}

BOOL Inject::createRemoteData(HANDLE hProcess, const wchar_t* gameExe,const wchar_t* dllName) {
	//分配内存
	LPVOID addrRemote = VirtualAllocEx(hProcess, 0, 0x3000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	



	RemoteData remoteData{};
	LPVOID remoteDataAddr = (LPVOID)((unsigned)addrRemote + 0x2000);
	LPVOID remoteProc = (LPVOID)((unsigned)addrRemote + 0x500);

	SIZE_T lwtd;
	//获取入口点
	remoteData.entryPoint = (unsigned)getEntryPoint(gameExe);
	remoteData.hookFunction = (unsigned)addrRemote;
	codeRemoteData(&remoteData, dllName);
	//写入远程数据
	WriteProcessMemory(hProcess, remoteDataAddr, &remoteData, sizeof(remoteData), &lwtd);


	//修正远程代码
	char _code[0x200];
	memcpy(_code, injectCode, sizeof(_code));
	for (int i = 0;i < 0x100; i++) {
		unsigned* pcode = (unsigned*)(&_code[i]);
		if (pcode[0] == 0xCCCCCCCC) {
			pcode[0] = (unsigned)remoteDataAddr;
			break;
		}
	}



	//0-0x200 注入代码
	// 0-0x500  远程代码区
	//0x2000 远程数据
	SIZE_T lwt;
	//写入内存
	WriteProcessMemory(hProcess, addrRemote, _code, 0x200, &lwt);
	WriteProcessMemory(hProcess, remoteProc, RemoteThreadProc, 0x200, &lwt);

	
	DWORD dwThreadId;
	HANDLE remoteHdl = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)remoteProc, remoteDataAddr, 0, &dwThreadId);
	WaitForSingleObject(remoteHdl, INFINITE);
	CloseHandle(remoteHdl);

	/**
	* 
	* 远程线程注入  测试
	DWORD dwThreadId;
	HANDLE remoteHdl = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)addrRemote, NULL, 0, &dwThreadId);
	WaitForSingleObject(remoteHdl, INFINITE);
	CloseHandle(remoteHdl);
	*/

	return TRUE;
}

void Inject::codeRemoteData(PRemoteData _data,const wchar_t* dllName) {
	short length;
	for (length = 0; dllName[length]; length++);
	HMODULE  hKernel = LoadLibrary(_T("kernel32.dll"));
	_data->f_loadLibrary = (_LoadLibrary)GetProcAddress(hKernel, "LoadLibraryW");
	_data->f_GetModuleHandleA = (_GetModuleHandleA)GetProcAddress(hKernel, "GetModuleHandleA");
	_data->f_VirtualProtect = (_VirtualProtect)GetProcAddress(hKernel, "VirtualProtect");
	memcpy(_data->dllName, dllName, (length + 1) * 2);
	//CString msg;
	//msg.Format(L"%X", _data->f_loadLibrary);
	//AfxMessageBox(msg);
}
