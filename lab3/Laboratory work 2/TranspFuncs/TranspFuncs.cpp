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
	::hWriteServer = GetStdHandle(STD_ERROR_HANDLE);
	::hReadServer = GetStdHandle(STD_INPUT_HANDLE);
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}

CTranspFuncsApp::~CTranspFuncsApp()
{
	CloseHandle(::hWriteServer);
	CloseHandle(::hReadServer);
}

// Единственный объект CTranspFuncsApp

CTranspFuncsApp theApp;


// Инициализация CTranspFuncsApp

BOOL CTranspFuncsApp::InitInstance()
{
	CWinApp::InitInstance();
	
	return TRUE;
}

size_t GetConfirm(HANDLE hPipe)
{
	return ReadFromPipe<size_t>(hPipe);
}

void SendNumberThread(size_t numberThread, HANDLE hPipe)
{
	WriteToPipe<size_t>(numberThread, hPipe);
}

void SendString(const string& str, HANDLE hPipe)
{
	WriteToPipe<string>(str, hPipe);
}

void __stdcall SendConfirm()
{
	WriteToPipe(size_t(0), hWriteServer);
}

void __stdcall SendConnectionRequest(const char* nameClient)
{	
	// Создаём трубы
	SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
	CreatePipe(&hReadServer, &hWriteClient, &sa, 0);
	CreatePipe(&hReadClient, &hWriteServer, &sa, 0);
	//SetHandleInformation(hWrite, HANDLE_FLAG_INHERIT, 0);
	//SetHandleInformation(hReadConfirm, HANDLE_FLAG_INHERIT, 0);
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = hReadServer;
	si.hStdOutput = 0;
	si.hStdError = hWriteServer;

	// Запускаем сервер
	PROCESS_INFORMATION pi;
	CreateProcess(NULL, (LPSTR)"Cplusplus.exe", &sa, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

	// Отправляем запрос на подключение
	SendCommand(string(nameClient), Command::Connection, hWriteClient);

	// Получаем подтверждение
	GetConfirm(::hReadClient);
}

void __stdcall StartThread(const char* nameClient)
{
	// Отправляем запрос на подключение
	SendCommand(string(nameClient), Command::Start, hWriteClient);

	// Получаем подтверждение
	GetConfirm(::hReadClient);
}

void __stdcall StopThread(const char* nameClient, int numberThread)
{
	// Отправляем запрос на подключение
	SendCommand(string(nameClient), Command::Stop, hWriteClient);

	// Отправляем номер потока
	SendNumberThread(size_t(numberThread), hWriteClient);

	// Получаем подтверждение
	GetConfirm(::hReadClient);
}

void __stdcall SendDisconnectRequest(const char* nameClient)
{
	// Отправляем запрос на разрыв соединения
	SendCommand(string(nameClient), Command::DisconnectingConnection, ::hWriteClient);

	// Получаем подтверждение
	GetConfirm(::hReadClient);

	CloseHandle(::hWriteClient);
	CloseHandle(::hReadClient);
}

void __stdcall CloseDialogue(const char* nameClient)
{
	// Отправляем запрос на разрыв соединения
	SendCommand(string(nameClient), Command::Close, ::hWriteClient);

	// Получаем подтверждение
	GetConfirm(::hReadClient);
}

void __stdcall SendCommand(const string& nameClient, Command command, HANDLE hPipe)
{
	WriteToPipe<string>(nameClient, hPipe);
	WriteToPipe<Command>(command, hPipe);
}

void __stdcall SendSMS(const char* nameClient, int numberThread, const char* message)
{
	// Отправляем запрос на подключение
	SendCommand(string(nameClient), Command::Send, hWriteClient);

	// Отправляем номер потока
	SendNumberThread(size_t(numberThread), hWriteClient);

	// Отправляем СМС
	SendString(string(message), hWriteClient);

	// Получаем подтверждение
	GetConfirm(::hReadClient);
}

void __stdcall SendMessageServer(const char* nameClient, const char* message)
{
	// Отправляем запрос на подключение
	SendCommand(string(nameClient), Command::SendServer, hWriteClient);

	// Отправляем СМС
	SendString(string(message), hWriteClient);

	// Получаем подтверждение
	GetConfirm(::hReadClient);
}

Command __stdcall ReceiveCommand(string& nameClient)
{
	nameClient = ReadFromPipe<string>(::hReadServer);
	return ReadFromPipe<Command>(::hReadServer);
}

size_t __stdcall ReceiveNumberThread()
{
	return ReadFromPipe<size_t>(::hReadServer);
}

string __stdcall ReceiveSMS()
{
	return ReadFromPipe<string>(::hReadServer);
}
