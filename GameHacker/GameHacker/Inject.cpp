#include "pch.h"
#include "Inject.h"


//ע�뺯��
void _stdcall injectCode() {
	unsigned address = 0xCCCCCCCC;
	PRemoteData p = (PRemoteData)address;
	p->f_loadLibrary(p->dllName);
	//LoadLibrary(L"E:\\c++\\GameHacker\\Release\\testdll.dll");
}


BOOL Inject::startProcess(CString _game_exe_file, CString _game_start_arg, CString _game_path, PROCESS_INFORMATION* proInfo) {

	
	STARTUPINFO startUpInfo{};
	startUpInfo.cb = sizeof(startUpInfo);
    

	game_exe_file = _game_exe_file;

	return  CreateProcess(
		_game_exe_file,
		_game_start_arg.GetBuffer(),
		NULL, NULL, TRUE,
		CREATE_SUSPENDED,
		NULL,
		_game_path,
		&startUpInfo,
		proInfo
	);



}

void* Inject::imageLoad(wchar_t* filename) {
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

DWORD Inject::getEntryPoint(wchar_t* filename) {
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

BOOL Inject::createRemoteData(HANDLE hProcess, wchar_t* dllName) {
	//�����ڴ�
	LPVOID addrRemote = VirtualAllocEx(hProcess, 0, 0x3000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	



	RemoteData _data{};
	LPVOID remoteDataAddr = (LPVOID)((unsigned)addrRemote + 0x2000);
	SIZE_T lwtd;
	codeRemoteData(&_data, dllName);
	//д��Զ������
	WriteProcessMemory(hProcess, remoteDataAddr, &_data, sizeof(_data), &lwtd);


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

	SIZE_T lwt;
	//д���ڴ�
	WriteProcessMemory(hProcess, addrRemote, _code, 0x200, &lwt);
	DWORD dwThreadId;
	HANDLE remoteHdl = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)addrRemote, NULL, 0, &dwThreadId);
	WaitForSingleObject(remoteHdl, INFINITE);
	return TRUE;
}

void Inject::codeRemoteData(PRemoteData _data, wchar_t* dllName) {
	short length;
	for (length = 0; dllName[length]; length++);
	HMODULE  hKernel = LoadLibrary(_T("kernel32.dll"));
	_data->f_loadLibrary = (_LoadLibrary)GetProcAddress(hKernel, "LoadLibraryW");
	memcpy(_data->dllName, dllName, (length + 1) * 2);
	//CString msg;
	//msg.Format(L"%X", _data->f_loadLibrary);
	//AfxMessageBox(msg);
}
