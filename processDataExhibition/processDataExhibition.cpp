// processDataProcessData.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.

#include <iostream>
#include <Windows.h>
#include <process.h> 
#include <sstream>

#include "../Utils/constants.h"

using namespace std;

HANDLE hProcessDataExhibitionEvent;
HANDLE hProcessDataThread;
HANDLE hExitEvent;
HANDLE hExitThread;
HANDLE hMailSlotReady;
HANDLE hFile;

unsigned __stdcall threadProcessData(void*);
unsigned __stdcall threadExit(void*);
string formatToExhibition(string message);

int main()
{
    cout << "Processo processDataExhibition iniciado. Esperando por evento..." << endl;

    hFile = CreateMailslot(mailProcess, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
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
    cout << "HH:MM:SS NSEQ:###### PR (T):######psi TEMP:######C PR (G):######psi NIVEL:######cm" << endl;
    cout << "==================================================================================" << endl;

    while (true) {
        WaitForSingleObject(hProcessDataExhibitionEvent, INFINITE);
        bool successRead = ReadFile(hFile, &msg, 100, &bytesRead, NULL);
        if (!successRead) {
            cout << "Falha na leitura de mailsot de dados de otimização" << endl;
            exit(1);
        }

        cout << formatToExhibition(msg) << endl;
    }
}

string formatToExhibition(string message) {
    string nSeq = message.substr(0, 6);
    string pressT = message.substr(10, 6);
    string temp = message.substr(17, 6);
    string pressG = message.substr(24, 6);
    string level = message.substr(31, 6);
    string time = message.substr(38);

    stringstream result;
    result << time << " NSEQ:" << nSeq << " PR (T):" << pressT << "psi TEMP:" << temp << "C PR (G):" << pressG << "psi NIVEL:" << level << "cm";

    return result.str();
}

unsigned __stdcall threadExit(void*) {
    hExitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, exitAll);
    if (!hExitEvent) {
        cout << "Erro ao buscar handle de exit. Erro ID: " << GetLastError() << endl;
    }
    WaitForSingleObject(hExitEvent, INFINITE);
    ExitProcess(0);
}