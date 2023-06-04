// TranspFuncs.h: основной файл заголовка для библиотеки DLL TranspFuncs
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы
#include <string>
#include <unordered_map>
#include <locale> 
#include <codecvt>

// CTranspFuncsApp
// Реализацию этого класса см. в файле TranspFuncs.cpp
//

using std::string;
using std::vector;
using std::unordered_map;
using std::wstring;

extern "C" {__declspec(dllexport) enum Command
{
	Connection = 0,
	DisconnectingConnection = 1,
	Start = 2,
	Stop = 3,
	Send = 4,
	Close = 5,
	SendServer = 6
}; }

class CTranspFuncsApp : public CWinApp
{
public:
	CTranspFuncsApp();
	~CTranspFuncsApp();
	
// Переопределение
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

};

HANDLE hWriteClient, hReadClient, hWriteServer, hReadServer;

size_t GetConfirm(HANDLE hPipe);

void SendNumberThread(size_t numberThread, HANDLE hPipe);

void SendString(const string& str, HANDLE hPipe);

void SendCommand(const string& nameClient, Command command, HANDLE hPipe);

extern "C" {__declspec(dllexport)  void __stdcall SendConnectionRequest(const char* nameClient); }

extern "C" {__declspec(dllexport)  void __stdcall StartThread(const char* nameClient); }

extern "C" {__declspec(dllexport)  void __stdcall StopThread(const char* nameClient, int numberThread); }

extern "C" {__declspec(dllexport)  void __stdcall SendSMS(const char* nameClient, int numberThread, const char* message); }

extern "C" {__declspec(dllexport)  void __stdcall SendMessageServer(const char* nameClient, const char* message); }

extern "C" {__declspec(dllexport)  void __stdcall SendDisconnectRequest(const char* nameClient); }

extern "C" {__declspec(dllexport)  void __stdcall CloseDialogue(const char* nameClient); }


__declspec(dllexport)  void __stdcall SendConfirm();

__declspec(dllexport)  Command __stdcall ReceiveCommand(string& nameClient);

__declspec(dllexport)  string __stdcall ReceiveSMS();

__declspec(dllexport)  size_t __stdcall ReceiveNumberThread();

template <typename T>
void WriteToPipe(const T& xType, HANDLE hPipe)
{
	DWORD dwBytesWrite;
	WriteFile(
		hPipe, // дескриптор файла
		&xType, // указатель на буфер данных
		sizeof(T), // количество записываемых байтов
		&dwBytesWrite, // количество записанных байтов
		(LPOVERLAPPED)NULL // используется при асинхронной записи
	);
	FlushFileBuffers(hPipe);
}

template <typename T>
T ReadFromPipe(HANDLE hPipe)
{
	T xType;
	DWORD dwBytesWrite;
	ReadFile(
		hPipe, // дескриптор файла
		&xType, // указатель на буфер данных
		sizeof(T), // количество читаемых байтов
		&dwBytesWrite, // количество прочитанных байтов
		(LPOVERLAPPED)NULL // используется при асинхронной записи
	);

	return xType;
}

template <>
void WriteToPipe<string>(const string& xType, HANDLE hPipe)
{
	DWORD dwBytesWrite;
	WriteToPipe<size_t>(xType.size(), hPipe);
	WriteFile(
		hPipe, // дескриптор файла
		xType.c_str(), // указатель на буфер данных
		xType.size() + 1, // количество записываемых байтов
		&dwBytesWrite, // количество записанных байтов
		(LPOVERLAPPED)NULL // используется при асинхронной записи
	);
	FlushFileBuffers(hPipe);
}

template <>
string ReadFromPipe<string>(HANDLE hPipe)
{
	size_t sz = ReadFromPipe<size_t>(hPipe);
	char* strBuf = new char[sz + 1]();
	strBuf[sz] = '\0';
	DWORD dwBytesWrite;
	ReadFile(
		hPipe, // дескриптор файла
		strBuf, // указатель на буфер данных
		sz + 1, // количество читаемых байтов
		&dwBytesWrite, // количество прочитанных байтов
		(LPOVERLAPPED)NULL // используется при асинхронной записи
	);
	string strBuf2 = string(strBuf);
	delete[] strBuf;
	return strBuf2;
}