#include <iostream>
#include <Windows.h>
#include <process.h> 

#include "../Utils/constants.h"

using namespace std;

HANDLE hOtimizationDataExhibitionEvent;
HANDLE hProcessDataThread;
HANDLE hExitEvent;
HANDLE hExitThread;

unsigned __stdcall threadOtimizationData(void*);
unsigned __stdcall threadExit(void*);

int main()
{
    cout << "Processo otimizationDataExhibition iniciado. Esperando por evento..." << endl;

    hOtimizationDataExhibitionEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, otimizationExhibition);
    if (hOtimizationDataExhibitionEvent == 0) {
        cout << "Falha em otimizationDataExhibition ..." << endl;
        exit(1);
    }

    hProcessDataThread = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadOtimizationData,
        NULL,
        0,
        NULL
    );
    if (hProcessDataThread == 0) {
        cout << "Falha em criação de thread de otimizationData ..." << endl;
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
        cout << "Falha em criação de thread de otimizationData ..." << endl;
        exit(1);
    }

    HANDLE handles[2] = { hProcessDataThread, hExitThread };

    WaitForMultipleObjects(2, handles, TRUE, INFINITE);
}

unsigned __stdcall threadOtimizationData(void*) {
    while (true) {
        WaitForSingleObject(hOtimizationDataExhibitionEvent, INFINITE);
        cout << "Thread de exibição de otimização de dados desbloqueada" << endl;
        // TODO: implementar comunicacao com processo gerador

        // TODO: remover sleep para nao imprimir o tempo todo quando for implementada a funcionalidade
        Sleep(2000);
    }
}

unsigned __stdcall threadExit(void*) {
    hExitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, exitAll);
    WaitForSingleObject(hExitEvent, INFINITE);
    ExitProcess(0);
}