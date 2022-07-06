// dataOtimizationAlarm.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.

#include <iostream>
#include <Windows.h>
#include <process.h> 

using namespace std;

HANDLE hAlarmExhibitionEvent;
HANDLE hAlarmThread;
HANDLE hExitEvent;
HANDLE hExitThread;

unsigned __stdcall threadAlarms(void*);
unsigned __stdcall threadExit(void *);

int main()
{
    cout << "Processo alarmExhibition iniciado. Esperando por evento..." << endl;

    hAlarmExhibitionEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Global\\alarmExhibitionEvent");
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
        cout << "Falha em criação de thread de alarmes ..." << endl;
        exit(1);
    }

    HANDLE handles[2] = { hAlarmThread, hExitThread };
    
    WaitForMultipleObjects(2, handles, TRUE, INFINITE);
}

unsigned __stdcall threadAlarms(void *) {
    while (true) {
        WaitForSingleObject(hAlarmExhibitionEvent, INFINITE);
        cout << "Thread de exibição de alarmes desbloqueada" << endl;
        // TODO: implementar comunicacao com processo gerador

        // TODO: remover sleep para nao imprimir o tempo todo quando for implementada a funcionalidade
        Sleep(2000);
    }
}

unsigned __stdcall threadExit(void *) {
    hExitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Global\\exitEvent");
    WaitForSingleObject(hExitEvent, INFINITE);
    ExitProcess(0);
}