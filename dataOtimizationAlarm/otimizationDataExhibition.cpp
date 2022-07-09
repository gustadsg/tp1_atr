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
HANDLE hFile;
HANDLE hSemaphoreHardDisk;

unsigned __stdcall threadOtimizationData(void*);
unsigned __stdcall threadExit(void*);
bool openSemaphores();

int main()
{
    cout << "Processo otimizationDataExhibition iniciado. Esperando por evento..." << endl;

    bool semaphoresOpened = openSemaphores();
    if (!semaphoresOpened) {
        cout << "Erro ao abrir semáforos" << endl;
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

    hFile = CreateFile(
        fileOtimization,
        GENERIC_WRITE | GENERIC_READ ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (!hFile)
    {
        cout << "Falha em criação de arquivo em disco otimização" << endl;
        exit(1);
    }

    HANDLE handles[2] = { hProcessDataThread, hExitThread };

    WaitForMultipleObjects(2, handles, TRUE, INFINITE);
}

unsigned __stdcall threadOtimizationData(void*) {
    DWORD bytesRead;
    char msg[40];
    long currentPosition = 0L;

    while (true) {
        WaitForSingleObject(hAlarmExhibitionEvent, INFINITE);
        WaitForSingleObject(hSemaphoreHardDisk, INFINITE);

        bool successRead = ReadFile(hFile, &msg, sizeof(msg), &bytesRead, NULL);
        if (!successRead) {
            int error = GetLastError();
            cout << "Falha na leitura de arquivo de disco de dados de otimização. Erro código: " << error << endl;
            exit(1);
        }
        cout << bytesRead << " bytes lidos. Msg: " << msg << endl;

        currentPosition++;
        if ((currentPosition % maxFileRows) == 0) {
            SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        }
    }
}

bool openSemaphores() {
    hSemaphoreHardDisk = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, hardDisksemaphore);
    return hSemaphoreHardDisk;
}

unsigned __stdcall threadExit(void*) {
    hExitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, exitAll);
    WaitForSingleObject(hExitEvent, INFINITE);
    ExitProcess(0);
}