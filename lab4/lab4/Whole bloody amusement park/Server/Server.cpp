// Laboratory work 1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include "pch.h"
#include "framework.h"
#include "Server.h"
#include <string>
#include <stack>
#include <iostream>
#include <mmsystem.h>
#include <unordered_map>
#include <filesystem>
#include <memory>
#include "../TranspFuncs/TranspFuncs.h"



#ifdef _DEBUG
#define new DEBUG_NEW                                   
#endif

using std::cout;
using std::endl;
using std::stack;
using std::string;
using std::to_string;
using std::to_wstring;
using std::unordered_map;
using std::pair;
using std::make_pair;
using std::shared_ptr;
using std::make_shared;

// Единственный объект приложения

CWinApp theApp;


struct ParamThread
{
    string whose;
    string* lastNameClientSenderCommand;
    string* SMS;
    size_t number;
    HANDLE hUniqueEventStartStop;
    HANDLE hUniqueEventSend;
    HANDLE hEventOut;
    CRITICAL_SECTION* cs;   
};

// Получить состояние события
bool getConditionEvent(HANDLE hEvent)
{
    if (WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Получить событие
size_t getEvent(DWORD nCount, const HANDLE* lpHandles)
{
    return static_cast<size_t>(WaitForMultipleObjects(nCount, lpHandles, FALSE, INFINITE) - WAIT_OBJECT_0);
}

string getStringValidThreads(vector<bool> threads)
{
    string bufStr;
    for (int i = 0; i < threads.size(); ++i)
    {
        if (threads[i])
        {
            bufStr += to_string(i + 1);
            bufStr += "|";
        }
    }
    if (bufStr.size() != 0)
    {
        bufStr.pop_back();

    }
    return bufStr;
}

void OutConsoleWithCS(const string& out, CRITICAL_SECTION* cs)
{
    EnterCriticalSection(cs);
    cout << out << endl;
    LeaveCriticalSection(cs);
}

UINT Thread(LPVOID lpParameters)
{
    ParamThread paramThread = *static_cast<ParamThread*>(lpParameters);
    WaitForSingleObject(paramThread.hUniqueEventStartStop, INFINITE);
    OutConsoleWithCS("Клиент: " + paramThread.whose + " - Старт потока: " + to_string(paramThread.number), paramThread.cs);
    SetEvent(paramThread.hEventOut);
    HANDLE lpHandles[2] = {paramThread.hUniqueEventStartStop, paramThread.hUniqueEventSend};
    HANDLE hFile;
    DWORD dwBytesWrite;
    While:
    switch(getEvent(2, lpHandles))
    {
    case 0:
    {
        goto ExitWhile;       
        break;
    }
    case 1:
    {
        string filePath = string(*(paramThread.lastNameClientSenderCommand) + "\\" + to_string(paramThread.number) + ".dat");
        hFile = CreateFile(filePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, 0);
        
        WriteFile(
                  hFile, // дескриптор файла
                  (*paramThread.SMS).c_str(), // адрес буфера, откуда идет запись
                  (*paramThread.SMS).size() + 1, // количество записываемых байтов
                  &dwBytesWrite, // количество записанных байтов
                  (LPOVERLAPPED)NULL); // запись синхронная
        CloseHandle(hFile);
        OutConsoleWithCS("Клиент: " + *(paramThread.lastNameClientSenderCommand) + " - поток: " + to_string(paramThread.number) + " - отправлена SMS: " + (*paramThread.SMS), paramThread.cs);
        SetEvent(paramThread.hEventOut);                
        break;
    }
    }
    goto While;

    ExitWhile:
    OutConsoleWithCS("Клиент: " + *(paramThread.lastNameClientSenderCommand) + " - Завершение потока: " + to_string(paramThread.number), paramThread.cs);
    SetEvent(paramThread.hEventOut);
    return 0;
}

void start()
{
    SetCurrentDirectory("../Csharp/bin/Debug");
    system("cls");
    SetConsoleTitleW(L"Сервер");    
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    

    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);
    
    HANDLE hEventOut = CreateEvent(NULL, FALSE, FALSE, "OutEvent");

    unordered_map<string, vector<HANDLE>> EventsThreads = { { "StartStop", vector<HANDLE>()},
                                                           { "Send", vector<HANDLE>()} };
                                                          
    vector<bool> threads = vector<bool>();
    bool isCloseServer = false;
    size_t numberThread;
    ParamThread Pt;
    string lastNameClientSenderCommand;
    string SMS;   
    while (!isCloseServer)
    {
        Pipe pipe = Pipe();
        switch (pipe.ReceiveCommand(lastNameClientSenderCommand))
        {
        case Connection:
        {           
            cout << "Соединение с клиентом: " + lastNameClientSenderCommand << endl;
            CreateDirectory((lastNameClientSenderCommand).c_str(), NULL);
            pipe.ReceiveConnectionRequest(std::move(getStringValidThreads(threads)));
            break;
        }
        case DisconnectingConnection:
        {
            cout << "Разрыв cоединения с клиентом: " + lastNameClientSenderCommand << endl;
            break;
        }
        case Start:
        {
            EventsThreads["StartStop"].push_back(CreateEvent(NULL, FALSE, FALSE, string("StartStop" + to_string(EventsThreads["StartStop"].size() + 1)).c_str()));
            EventsThreads["Send"].push_back(CreateEvent(NULL, FALSE, FALSE, string("Send" + to_string(EventsThreads["Send"].size() + 1)).c_str()));
            Pt = ParamThread(lastNameClientSenderCommand,
                             &lastNameClientSenderCommand,
                             &SMS,
                             threads.size() + 1,
                             EventsThreads["StartStop"].back(),
                             EventsThreads["Send"].back(),
                             hEventOut, &cs);
            AfxBeginThread(Thread, (LPVOID*)&Pt);
            threads.push_back(true);

            // Даём отмашку потоку на старт
            SetEvent(EventsThreads["StartStop"].back());

            // Ждем пока поток выведет сообщение о своём создании
            WaitForSingleObject(hEventOut, INFINITE);
                                                                                                                 
            break;                                                           
        }
        case Stop:                                                              
        {                                                                                                                                                                                                              
        
            pipe.SendUpdateInfo(std::move(getStringValidThreads(threads)));

            numberThread = pipe.ReceiveNumberThread();

            if (numberThread)
            {
                // Даём отмашку на завершение потока
                SetEvent(EventsThreads["StartStop"][numberThread - 1]);

                // Ждем пока поток выведет сообщение о своём завершении
                WaitForSingleObject(hEventOut, INFINITE);

                // Подчищаем событие закрепленное за потоком
                CloseHandle(EventsThreads["StartStop"][numberThread - 1]);
                CloseHandle(EventsThreads["Send"][numberThread - 1]);

                threads[numberThread - 1] = false;
            }

            break;
        }
        case StopAll:
        {
            for (int i = 0; i < threads.size(); ++i)
            {
                if (threads[i])
                {
                    // Даём отмашку на завершение потока
                    SetEvent(EventsThreads["StartStop"][i]);

                    // Ждем пока поток выведет сообщение о своём завершении
                    WaitForSingleObject(hEventOut, INFINITE);

                    // Подчищаем событие закрепленное за потоком
                    CloseHandle(EventsThreads["StartStop"][i]);
                    CloseHandle(EventsThreads["Send"][i]);

                    threads[i] = false;
                }
            }

            break;
        }
        case Send:
        {
            pipe.SendUpdateInfo(std::move(getStringValidThreads(threads)));

            numberThread = pipe.ReceiveNumberThread();

            if (numberThread)
            {
                SMS = pipe.ReceiveSMS();

                SetEvent(EventsThreads["Send"][numberThread - 1]);

                WaitForSingleObject(hEventOut, INFINITE);
            }

            break;
        }
        case SendMessageServer:
        {
            ResetEvent(hEventOut);
            OutConsoleWithCS("Клиент: " + lastNameClientSenderCommand + " - Сообщение серверу: " + pipe.ReceiveSMS(), &cs);
            SetEvent(hEventOut); 

            break;
        }

        case SendAll:
        {
            SMS = pipe.ReceiveSMS();

            ResetEvent(hEventOut);
            OutConsoleWithCS("Клиент: " + lastNameClientSenderCommand + " - Сообщение серверу: " + SMS, &cs);
            SetEvent(hEventOut);

            for (int i = 0; i < threads.size(); ++i)
            {
                if (threads[i])
                {
                    // Даём отмашку на завершение потока
                    SetEvent(EventsThreads["Send"][i]);

                    // Ждем пока поток выведет сообщение о своём завершении
                    WaitForSingleObject(hEventOut, INFINITE);
                }
            }

            break;
        }
        case CloseDialogue:
        {
            for (int i = 0; i < threads.size(); ++i)
            {
                if (threads[i])
                {
                    // Даём отмашку на завершение потока
                    SetEvent(EventsThreads["StartStop"][i]);

                    // Ждем пока поток выведет сообщение о своём завершении
                    WaitForSingleObject(hEventOut, INFINITE);

                    // Подчищаем событие закрепленное за потоком
                    CloseHandle(EventsThreads["StartStop"][i]);
                    CloseHandle(EventsThreads["Send"][i]);

                    threads[i] = false;
                }
            }

            isCloseServer = true;

            break;     
        }
        case UpdateInfo:
        {
            pipe.SendUpdateInfo(std::move(getStringValidThreads(threads)));

            break;
        }
        }
        pipe.~Pipe();
    }
    
    CloseHandle(hEventOut);
}

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: вставьте сюда код для приложения.
            start();
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}

