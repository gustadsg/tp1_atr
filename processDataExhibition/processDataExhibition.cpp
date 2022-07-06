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

unsigned __stdcall threadProcessData(void*);
unsigned __stdcall threadExit(void*);

int main()
{
    cout << "Processo processDataExhibition iniciado. Esperando por evento..." << endl;

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
    while (true) {
        WaitForSingleObject(hProcessDataExhibitionEvent, INFINITE);
        cout << "Thread de exibição de processamento de dados desbloqueada" << endl;
        // TODO: implementar comunicacao com processo gerador

        // TODO: remover sleep para nao imprimir o tempo todo quando for implementada a funcionalidade
        Sleep(2000);
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