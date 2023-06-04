// TranspFuncs.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "TranspFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma comment(lib, "TranspFuncs.lib")
//
//TODO: если эта библиотека DLL динамически связана с библиотеками DLL MFC,
//		все функции, экспортированные из данной DLL-библиотеки, которые выполняют вызовы к
//		MFC, должны содержать макрос AFX_MANAGE_STATE в
//		самое начало функции.
//
//		Например:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// тело нормальной функции
//		}
//
//		Важно, чтобы данный макрос был представлен в каждой
//		функции до вызова MFC.  Это означает, что
//		должен стоять в качестве первого оператора в
//		функции и предшествовать даже любым объявлениям переменных объекта,
//		поскольку их конструкторы могут выполнять вызовы к MFC
//		DLL.
//
//		В Технических указаниях MFC 33 и 58 содержатся более
//		подробные сведения.
//

// CTranspFuncsApp

BEGIN_MESSAGE_MAP(CTranspFuncsApp, CWinApp)
END_MESSAGE_MAP()


// Создание CTranspFuncsApp

CTranspFuncsApp::CTranspFuncsApp()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}




// Единственный объект CTranspFuncsApp

CTranspFuncsApp theApp;


// Инициализация CTranspFuncsApp

BOOL CTranspFuncsApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}

CTranspFuncsApp::~CTranspFuncsApp()
{

}

Pipe::Pipe() : hPipe(CreateNamedPipe(TEXT("\\\\.\\pipe\\MyPipe"),
	PIPE_ACCESS_DUPLEX,
	PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
	PIPE_UNLIMITED_INSTANCES,
	1024, 1024, 0, NULL))
{
	
	ConnectNamedPipe(hPipe, NULL);
}

void Pipe::JoinPipe()
{
	WaitNamedPipe(TEXT("\\\\.\\pipe\\MyPipe"), NMPWAIT_WAIT_FOREVER);

	::hPipe = CreateFile(TEXT("\\\\.\\pipe\\MyPipe"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
}

void Pipe::LeavePipe()
{
	CloseHandle(::hPipe);
}

Pipe::~Pipe()
{
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
}

//void __stdcall CommandReceived()
//{
//	ClosePipe();
//}

void Pipe::SendCommand(const string& nameClient, Command command, HANDLE hPipe)
{
	WriteToPipe<string>(nameClient, hPipe);

	WriteToPipe<Command>(command, hPipe);
}

void Pipe::SendSize_t(size_t sz_t, HANDLE hPipe)
{
	WriteToPipe<size_t>(sz_t, hPipe);

}

size_t Pipe::getSize_t()
{
	return ReadFromPipe<size_t>(hPipe);
}

void Pipe::SendString(const string& str, HANDLE hPipe)
{
	WriteToPipe<string>(str, hPipe);
}

string Pipe::getString()
{
	return ReadFromPipe<string>(hPipe);
}

char* Pipe::getUpdateInfo(char* updateInfo)
{
	string bufStr = ReadFromPipe<string>(::hPipe);
	strcpy(updateInfo, bufStr.c_str());
	return updateInfo;
}

bool Pipe::CheckThreads(size_t numberThread)
{
	bool stop = false;
	string strBuf = string(3000, '\0');
	strBuf = string(getUpdateInfo(const_cast<char*>(strBuf.c_str())));
	strBuf.push_back('|');
	{
		int i = 0;
		int j = 0;
		for (; i < strBuf.size(); ++i)
		{
			if (strBuf[i] == '|')
			{
				if (stoi(strBuf.substr(j, i - j)) == numberThread)
				{
					stop = true;
					break;
				}
				j = i + 1;
			}
		}
	}

	return stop;
}

void Pipe::SendUpdateInfo(const string& updateInfo)
{
	WriteToPipe<string>(updateInfo, hPipe);
}

char* __stdcall SendConnectionRequest(const char* nameClient, char* updateInfo)
{	
	Pipe::JoinPipe();
	
	Pipe::SendCommand(string(nameClient), Command::Connection, hPipe);

	Pipe::getUpdateInfo(updateInfo);

	Pipe::LeavePipe();

	return updateInfo;
}

void __stdcall SendDisconnectRequest(const char* nameClient)
{
	Pipe::JoinPipe();

	Pipe::SendCommand(string(nameClient), Command::DisconnectingConnection, hPipe);

	CloseHandle(hPipe);	
}

void __stdcall StartThread(const char* nameClient)
{
	Pipe::JoinPipe();

	Pipe::SendCommand(string(nameClient), Command::Start, hPipe);

	Pipe::LeavePipe();
}

bool __stdcall StopThread(const char* nameClient, int numberThread)
{
	Pipe::JoinPipe();

	Pipe::SendCommand(string(nameClient), Command::Stop, hPipe);

	bool stop = Pipe::CheckThreads(numberThread);
	
	if (stop)
	{
		Pipe::SendSize_t(numberThread, hPipe);
		
	}
	else
	{
		Pipe::SendSize_t(0, hPipe);
	}

	Pipe::LeavePipe();

	return stop;
}

void __stdcall StopAllThreads(const char* nameClient)
{

	Pipe::JoinPipe();

	Pipe::SendCommand(string(nameClient), Command::StopAll, hPipe);
	
	Pipe::LeavePipe();
}

char* __stdcall Update(const char* nameClient, char* updateInfo)
{
	Pipe::JoinPipe();

	Pipe::SendCommand(string(nameClient), Command::UpdateInfo, hPipe);
	
	Pipe::getUpdateInfo(updateInfo);

	Pipe::LeavePipe();

	return updateInfo;
}

bool __stdcall SendSMS(const char* nameClient, int numberThread, const char* message)
{
	Pipe::JoinPipe();

	Pipe::SendCommand(string(nameClient), Command::Send, hPipe);

	bool send = Pipe::CheckThreads(numberThread);

	if (send)
	{
		Pipe::SendSize_t(numberThread, hPipe);
		Pipe::SendString(string(message), hPipe);
	}
	else
	{
		Pipe::SendSize_t(0, hPipe);
	}

	Pipe::LeavePipe();

	return send;
}

void __stdcall SendSMSToServer(const char* nameClient, const char* message)
{
	Pipe::JoinPipe();

	Pipe::SendCommand(string(nameClient), Command::SendMessageServer, hPipe);

	Pipe::SendString(string(message), hPipe);

	Pipe::LeavePipe();
}

void __stdcall SendSMSAll(const char* nameClient, const char* message)
{
	Pipe::JoinPipe();

	Pipe::SendCommand(string(nameClient), Command::SendAll, hPipe);

	Pipe::SendString(string(message), hPipe);

	Pipe::LeavePipe();
}

void __stdcall CloseServer()
{
	Pipe::JoinPipe();

	Pipe::SendCommand(" ", Command::CloseDialogue, hPipe);

	Pipe::LeavePipe();
}

void Pipe::ReceiveConnectionRequest(const string& updateInfo)
{
	SendUpdateInfo(updateInfo);
}

Command Pipe::ReceiveCommand(string& nameClient)
{
	nameClient = getString();

	Command commandBuf = ReadFromPipe<Command>(hPipe);
		
	return commandBuf;
}

size_t Pipe::ReceiveNumberThread()
{
	return getSize_t();
}

string Pipe::ReceiveSMS()
{
	return getString();
}








