// processDataProcessData.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.

#include <iostream>
#include <Windows.h>
#include <process.h> 

#include "../Utils/constants.h"

using namespace std;

HANDLE hProcessDataExhibitionEvent;
HANDLE hProcessDataThread;
HANDLE hExitEvent;
HANDLE hExitThread;
HANDLE hMailSlotReady;
HANDLE hMailslot;

unsigned __stdcall threadProcessData(void*);
unsigned __stdcall threadExit(void*);

int main()
{
    cout << "Processo processDataExhibition iniciado. Esperando por evento..." << endl;

    hMailslot = CreateMailslot(mailProcess, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE)
    {
        cout << "Falha em criação de mailslot" << endl;
        exit(1);
    }

    hMailSlotReady = OpenEvent(EVENT_ALL_ACCESS, FALSE, mailProcessReady);
    if (!hMailSlotReady) {
        cout << "Falha em criação de evento de mailslot de dados de otimização" << endl;
        exit(1);
    }

    hProcessDataExhibitionEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, processExhibition);
    if (hProcessDataExhibitionEvent == 0) {
        cout << "Falha em processDataExhibition ..." << endl;
        exit(1);
    }

    hProcessDataThread = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadProcessData,
        NULL,
        0,
        NULL
    );
    if (hProcessDataThread == 0) {
        cout << "Falha em criação de thread de processDataes ..." << endl;
        exit(1);
    }

    hExitThread = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadExit,
        NULL,
        0,
        NULL
    );
    if (hExitThread == 0) {
        cout << "Falha em criação de thread de processData ..." << endl;
        exit(1);
    }

    HANDLE handles[2] = { hProcessDataThread, hExitThread };

    WaitForMultipleObjects(2, handles, TRUE, INFINITE);
}

unsigned __stdcall threadProcessData(void*) {
    DWORD bytesRead;
    char msg[100];

    SetEvent(hMailSlotReady);

    while (true) {
        WaitForSingleObject(hProcessDataExhibitionEvent, INFINITE);
        bool successRead = ReadFile(hMailslot, &msg, 100, &bytesRead, NULL);
        if (!successRead) {
            cout << "Falha na leitura de mailsot de dados de otimização" << endl;
            exit(1);
        }

        cout << bytesRead << " bytes lidos. Msg: " << msg << endl;
    }
}

unsigned __stdcall threadExit(void*) {
    hExitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, exitAll);
    if (!hExitEvent) {
        cout << "Erro ao buscar handle de exit. Erro ID: " << GetLastError() << endl;
    }
    WaitForSingleObject(hExitEvent, INFINITE);
    ExitProcess(0);
}