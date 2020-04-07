#include<Windows.h>
#include<stdio.h>

int main() {

	WCHAR name[] = L"C:\\Windows\\explorer.exe";		// Process name of which you want to change PPID
	DWORD ParentID = 8588;								// Existing PPID  

	HANDLE hParent = ::OpenProcess(PROCESS_CREATE_PROCESS, FALSE, ParentID);
	if (!hParent) return 0;

	PROCESS_INFORMATION pi = { 0 };
	PPROC_THREAD_ATTRIBUTE_LIST attlist = nullptr;

	do {
		SIZE_T size;
		::InitializeProcThreadAttributeList(nullptr, 1, 0, &size);
		if (size == 0) break;
		attlist = (PPROC_THREAD_ATTRIBUTE_LIST)malloc(size);
		if (!attlist) break;
		if (!::InitializeProcThreadAttributeList(attlist, 1, 0, &size)) break;
		if (!::UpdateProcThreadAttribute(attlist, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS,		// This attribute is responsible to change the parent process id
			&hParent, sizeof(hParent), nullptr, nullptr)) break;

		STARTUPINFOEX si = {sizeof(si)};
		si.lpAttributeList = attlist;
		if (!::CreateProcess(nullptr, name, nullptr, nullptr, FALSE, EXTENDED_STARTUPINFO_PRESENT | CREATE_NEW_CONSOLE, nullptr, nullptr, (STARTUPINFO*)&si, &pi)) printf("Error: %d", ::GetLastError());
		::WaitForSingleObject(pi.hProcess, INFINITE);
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
	} while (false);

	::CloseHandle(hParent);
	if (attlist) {
		::DeleteProcThreadAttributeList(attlist);
		::free(attlist);
	}

	return 0;
}