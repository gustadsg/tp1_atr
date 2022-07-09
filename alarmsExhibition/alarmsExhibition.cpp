// dataOtimizationAlarm.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.

#include <iostream>
#include <Windows.h>
#include <process.h> 

#include "../Utils/constants.h"

using namespace std;

HANDLE hAlarmExhibitionEvent;
HANDLE hAlarmThread;
HANDLE hExitEvent;
HANDLE hExitThread;
HANDLE hClearConsoleThread;
HANDLE hMailslot;
HANDLE hMailSlotReady;
HANDLE hClearConsole;


unsigned __stdcall threadAlarms(void*);
unsigned __stdcall threadExit(void *);
unsigned __stdcall threadClearConsole(void*);

int main()
{
    cout << "Processo alarmExhibition iniciado. Esperando por evento..." << endl;

    hMailslot = CreateMailslot(mailAlarm, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE)
    {
        cout << "Falha em criação de mailslot" << endl;
        exit(1);
    }

    hMailSlotReady = OpenEvent(EVENT_ALL_ACCESS, FALSE, mailAlarmReady);
    if (!hMailSlotReady) {
        cout << "Falha em criação de evento de mailslot de dados de otimização" << endl;
        exit(1);
    }

    hAlarmExhibitionEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, alarmExhibition);
    if (hAlarmExhibitionEvent == 0) {
        cout << "Falha em alarmExhibition ..." << endl;
        exit(1);
    }


    hAlarmThread = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadAlarms,
        NULL,
        0,
        NULL
    );
    if (hAlarmThread == 0) {
        cout << "Falha em criação de thread de alarmes ..." << endl;
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
        cout << "Falha em criação de thread de exit..." << endl;
        exit(1);
    }

    hClearConsoleThread = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadClearConsole,
        NULL,
        0,
        NULL
    );
    if (hClearConsoleThread == 0) {
        cout << "Falha em criação de thread de limpeza de console ..." << endl;
        exit(1);
    }

    HANDLE handles[2] = { hAlarmThread, hExitThread };
    
    WaitForMultipleObjects(2, handles, TRUE, INFINITE);
}

unsigned __stdcall threadAlarms(void *) {
    DWORD bytesRead;
    char msg[100];

    SetEvent(hMailSlotReady);

    while (true) {
        WaitForSingleObject(hAlarmExhibitionEvent, INFINITE);
        bool successRead = ReadFile(hMailslot, &msg, 100, &bytesRead, NULL);
        if (!successRead) {
            cout << "Falha na leitura de mailsot de dados de otimização" << endl;
            exit(1);
        }

        cout << bytesRead << " bytes lidos. Msg: " << msg << endl;
    }
}

unsigned __stdcall threadExit(void *) {
    hExitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, exitAll);
    WaitForSingleObject(hExitEvent, INFINITE);
    ExitProcess(0);
}

unsigned __stdcall threadClearConsole(void*) {
    hClearConsole = OpenEvent(EVENT_ALL_ACCESS, FALSE, clearAlarmConsole);
    while (true) {
        WaitForSingleObject(hClearConsole, INFINITE);
        system("cls");
    }
}