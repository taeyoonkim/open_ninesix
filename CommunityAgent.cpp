#include "stdafx.h"
#include "CommunityAgent.h"
#include <tchar.h>
#include "Service.h"
#include <psapi.h>
#include <WinUser.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//taeyoon kim

CWinApp theApp;
SOCKET server;

using namespace std;

UINT  AgentThread(LPVOID pParam);
//UINT  ClientThread(LPVOID pParam);
BOOL ParseCmd(char *str, CString& cmd, CString& params);
BOOL SendFile(SOCKET s, CString fname);

static TCHAR g_szFilePath[MAX_PATH];

static LPTSTR GetFilePart(LPTSTR source)
{
	LPTSTR result = _tcsrchr(source, '\\');
	if (result)
		result++;
	else
		result = source;
	return result;
}

typedef struct _tagEnumWndData {
	DWORD_PTR	pid;
	HWND	hwndFind;
} ENUMWINDOWDATA;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	return FALSE;
}
HWND FindWindow( HANDLE hProcess)
{
	ENUMWINDOWDATA ewd;
	ewd.hwndFind = NULL;

#if 1 // windows 2003 or higher
	ewd.pid = GetProcessId( hProcess);
	EnumWindows( (WNDENUMPROC) EnumWindowsProc, reinterpret_cast<LPARAM>(&ewd));
#else
	ewd.pid = ntProc.GetProcessIdNT( hProcess);
	EnumWindows( (WNDENUMPROC) EnumWindowsProc, reinterpret_cast<LPARAM>(&ewd));
#endif
	return ewd.hwndFind;
}

DWORD FindProcessByPath( LPCTSTR lpszPathName)
{
	DWORD aProcesses[1024], cbNeeded;

	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		return NULL;
	DWORD cProcesses = cbNeeded / sizeof(DWORD);

	for ( UINT i = 0; i < cProcesses; i++ )
	{
		HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, aProcesses[i] );
		if( hProcess )
		{
			char szImageFileName[MAX_PATH];
			DWORD dwLen = GetProcessImageFileName( hProcess, szImageFileName, sizeof(szImageFileName));
			CloseHandle( hProcess );

			if( dwLen )
			{
				char szTarget[80];
				dwLen = QueryDosDevice( CString(lpszPathName, 2), szTarget, sizeof(szTarget));

				if( _strnicmp( szTarget, szImageFileName, strlen(szTarget)) == 0)
				{
					if( NULL !=strstr(szImageFileName,"CommunityServer.exe") )
                        return aProcesses[i];
				}
			}
		}
	}

	return NULL;
}

BOOL RunCommand(int nCmd )
{
	return FALSE;
}

class CUpdateService: public CService
{
public:
	VOID Main(void)
	{
		CService::Begin(_T("updateServer"));
		CService::End();
	}
protected:
	void OnStarted()
	{
		AfxBeginThread(AgentThread,0);
	}
	void OnStopped()
	{
		DWORD dwProcessId = FindProcessByPath( g_szFilePath );
		HANDLE hProcess;
		if( dwProcessId && (hProcess = OpenProcess( PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwProcessId)) )
		{

			if( TerminateProcess( hProcess, -19))
			{
			}
			CloseHandle( hProcess);
		} 

		closesocket(server);
		WSACleanup();
	}
};

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{

	GetModuleFileName(0, g_szFilePath, sizeof(g_szFilePath));
	LPTSTR FilePart = GetFilePart(g_szFilePath);
	_tcscpy_s(FilePart, g_szFilePath + MAX_PATH - FilePart, _T("CommunityServer.exe"));

	CUpdateService upServ;
	if(argc == 2)
	{
		if(_tcscmp(argv[1],_T("-i")) == 0)
		{
			//upServ.Install(_T("CommunityAgent"));
		}
		else if(_tcscmp(argv[1],_T("-u")) == 0)
		{
			//upServ.Uninstall(_T("CommunityAgent"));
		}
		return TRUE;
	}

	HANDLE m_hMutex = CreateMutex(NULL, FALSE, "CommunityAgent");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;

		return FALSE;
	}

	upServ.Main();

	return TRUE;
	
}


UINT  AgentThread(LPVOID pParam)
{		
	
	TCHAR chDirPath[MAX_PATH];

	while(1)
	{
		DWORD dwProcessId = FindProcessByPath( g_szFilePath );
		if( dwProcessId == NULL )
		{
			PROCESS_INFORMATION piProcInfo;
			STARTUPINFO siStartupInfo;

			siStartupInfo.cb = sizeof(STARTUPINFO);
			siStartupInfo.lpReserved = NULL;
			siStartupInfo.lpDesktop = NULL;
			siStartupInfo.lpTitle = NULL;
			siStartupInfo.dwFlags = STARTF_USESHOWWINDOW;
			siStartupInfo.wShowWindow = SW_SHOWNORMAL;
			siStartupInfo.cbReserved2 = 0;
			siStartupInfo.lpReserved2 = NULL;

			strcpy_s(chDirPath,g_szFilePath);

			CString strModulePath( chDirPath );
			PathRemoveFileSpec( chDirPath );

			if( CreateProcess( NULL, strModulePath.GetBuffer(0), NULL, NULL, false, CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS,
				NULL, chDirPath, &siStartupInfo, &piProcInfo) )
			{
			} 
		} 

		Sleep(3000);
	}
	
	return 0;
}

UINT  ClientThread(LPVOID pParam)
{
	
	return 0;
}

BOOL ParseCmd(char *str, CString& cmd, CString& params)
{

	return true;
}

BOOL SendFile(SOCKET s, CString fname)
{	
	return true;
}