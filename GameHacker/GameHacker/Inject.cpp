#include "pch.h"
#include "Inject.h"


//ע�뺯��
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
	unsigned base = p->f_GetModuleHandleA(0);  //��ȡ�ڴ��ַ
	DWORD dRet;
	p->entryPoint += base;   // 400000 + 25D20
	p->f_VirtualProtect((LPVOID)p->entryPoint, 0x5, PAGE_EXECUTE_READWRITE, &dRet);
	char* entryCode = (char*)p->entryPoint;
	//������ڵ�
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
	//�Զ����Ƶ���ʽ��ȡ
	std::ifstream steamRead(filename, std::ios::binary);
	//��ת��β��
	steamRead.seekg(0, std::ios::end);
	//��ȡ�ļ���С
	unsigned filesize = steamRead.tellg();
	//�����ڴ�
	char* _data = new char[filesize];
	//��ת��ͷ��
	steamRead.seekg(0, std::ios::beg);
	//��ȡ���ڴ�
	steamRead.read(_data, filesize);
	//�ر�
	steamRead.close();
	return _data;

}

void Inject::unLoadImage(void* _data) {
	delete[] _data;
}

DWORD Inject::getEntryPoint(const wchar_t* filename) {
	void* image = imageLoad(filename);
	//��ȡdosͷ
	IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)image;
	//pe��ַ
	unsigned PEAddress = dosHeader->e_lfanew + (unsigned)image;
	//��ȡntͷ
	IMAGE_NT_HEADERS* ntHeader = (IMAGE_NT_HEADERS*)PEAddress;
	//h��ȡ��ڵ�
	DWORD entryPoint = ntHeader->OptionalHeader.AddressOfEntryPoint;
	unLoadImage(image);
	return entryPoint;
}

BOOL Inject::createRemoteData(HANDLE hProcess, const wchar_t* gameExe,const wchar_t* dllName) {
	//�����ڴ�
	LPVOID addrRemote = VirtualAllocEx(hProcess, 0, 0x3000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	



	RemoteData remoteData{};
	LPVOID remoteDataAddr = (LPVOID)((unsigned)addrRemote + 0x2000);
	LPVOID remoteProc = (LPVOID)((unsigned)addrRemote + 0x500);

	SIZE_T lwtd;
	//��ȡ��ڵ�
	remoteData.entryPoint = (unsigned)getEntryPoint(gameExe);
	remoteData.hookFunction = (unsigned)addrRemote;
	codeRemoteData(&remoteData, dllName);
	//д��Զ������
	WriteProcessMemory(hProcess, remoteDataAddr, &remoteData, sizeof(remoteData), &lwtd);


	//����Զ�̴���
	char _code[0x200];
	memcpy(_code, injectCode, sizeof(_code));
	for (int i = 0;i < 0x100; i++) {
		unsigned* pcode = (unsigned*)(&_code[i]);
		if (pcode[0] == 0xCCCCCCCC) {
			pcode[0] = (unsigned)remoteDataAddr;
			break;
		}
	}



	//0-0x200 ע�����
	// 0-0x500  Զ�̴�����
	//0x2000 Զ������
	SIZE_T lwt;
	//д���ڴ�
	WriteProcessMemory(hProcess, addrRemote, _code, 0x200, &lwt);
	WriteProcessMemory(hProcess, remoteProc, RemoteThreadProc, 0x200, &lwt);

	
	DWORD dwThreadId;
	HANDLE remoteHdl = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)remoteProc, remoteDataAddr, 0, &dwThreadId);
	WaitForSingleObject(remoteHdl, INFINITE);
	CloseHandle(remoteHdl);

	/**
	* 
	* Զ���߳�ע��  ����
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
