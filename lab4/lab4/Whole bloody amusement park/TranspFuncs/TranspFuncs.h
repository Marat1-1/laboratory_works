// TranspFuncs.h: основной файл заголовка для библиотеки DLL TranspFuncs
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
#include <string.h>
#include "resource.h"		// основные символы
#include <string>
#include <unordered_map>
#include <locale> 
#include <codecvt>
#include <cstring>
// CTranspFuncsApp
// Реализацию этого класса см. в файле TranspFuncs.cpp
//

using std::pair;
using std::make_pair;
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
	CloseDialogue = 5,
	SendMessageServer = 6,
	UpdateInfo = 7,
	StopAll = 8,
	SendAll = 9
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

HANDLE hPipe;

 class __declspec(dllexport)  Pipe
{
	private:
		HANDLE hPipe;

		size_t getSize_t();
		string getString();
		
	public:
		Pipe();
		static void JoinPipe();
		static void LeavePipe();
		static void SendSize_t(size_t sz_t, HANDLE hPipe);
		static void SendCommand(const string& nameClient, Command command, HANDLE hPipe);
		static void SendString(const string& str, HANDLE hPipe);
		static char* getUpdateInfo(char* updateInfo);
		static bool CheckThreads(size_t numberThread);
		void SendUpdateInfo(const string& updateInfo);
		void ReceiveConnectionRequest(const string& updateInfo);
		Command ReceiveCommand(string& nameClient);
		string ReceiveSMS();
		size_t ReceiveNumberThread();
		~Pipe();
};

extern "C" {__declspec(dllexport)  char* __stdcall SendConnectionRequest(const char* nameClient, char* updateInfo); }

extern "C" {__declspec(dllexport)  void __stdcall StartThread(const char* nameClient); }

extern "C" {__declspec(dllexport)  bool __stdcall StopThread(const char* nameClient, int numberThread); }

extern "C" {__declspec(dllexport)  void __stdcall StopAllThreads(const char* nameClient); }

extern "C" {__declspec(dllexport)  char* __stdcall Update(const char* nameClient, char* updateInfo); }

extern "C" {__declspec(dllexport)  bool __stdcall SendSMS(const char* nameClient, int numberThread, const char* message); }

extern "C" {__declspec(dllexport)  void __stdcall SendSMSToServer(const char* nameClient, const char* message); }

extern "C" {__declspec(dllexport)  void __stdcall SendSMSAll(const char* nameClient, const char* message); }

extern "C" {__declspec(dllexport)  void __stdcall SendDisconnectRequest(const char* nameClient); }

extern "C" {__declspec(dllexport)  void __stdcall CloseServer(); }

string getLastError()
{
	DWORD ErrorID;
	ErrorID = GetLastError();
	LPSTR pBuff = nullptr;
	size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, ErrorID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&pBuff, 0, NULL);
	string s(pBuff, size);
	LocalFree(pBuff);
	return s;
}

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
	string bufStr = getLastError();
	string strBuf2 = string(strBuf);
	delete[] strBuf;
	return strBuf2;
}








