#include <iostream>
#include <Windows.h>
#include <process.h> 

#include "../Utils/constants.h"
#include "../Utils/Message.h"

using namespace std;

HANDLE hAlarmExhibitionEvent;
HANDLE hProcessDataThread;
HANDLE hExitEvent;
HANDLE hExitThread;
HANDLE hMailslot;
HANDLE hMailSlotReady;

unsigned __stdcall threadOtimizationData(void*);
unsigned __stdcall threadExit(void*);

int main()
{
    cout << "Processo otimizationDataExhibition iniciado. Esperando por evento..." << endl;

    hMailSlotReady = OpenEvent(EVENT_ALL_ACCESS, FALSE, mailOtimizationReady);
    if (!hMailSlotReady) {
        cout << "Falha em criação de evento de mailslot de dados de otimização" << endl;
        exit(1);
    }

    hMailslot = CreateMailslot(mailOtimization, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE)
    {
        cout << "Falha em criação de mailslot" << endl;
        exit(1);
    }

    hAlarmExhibitionEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, otimizationExhibition);
    if (hAlarmExhibitionEvent == 0) {
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

unsigned __stdcall threadExit(void*) {
    hExitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, exitAll);
    WaitForSingleObject(hExitEvent, INFINITE);
    ExitProcess(0);
}