// dataOtimizationAlarm.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.

#include <iostream>
#include <Windows.h>
#include <process.h> 
#include <sstream> 

#include "../Utils/constants.h"

using namespace std;

HANDLE hAlarmExhibitionEvent;
HANDLE hAlarmThread;
HANDLE hExitEvent;
HANDLE hExitThread;
HANDLE hClearConsoleThread;
HANDLE hFile;
HANDLE hMailSlotReady;
HANDLE hClearConsole;


unsigned __stdcall threadAlarms(void*);
unsigned __stdcall threadExit(void *);
unsigned __stdcall threadClearConsole(void*);
string formatToExhibition(string input);
string mapIdToText(string ID);
string fillWithWhitespaces(string str, int size);

int main()
{
    cout << "Processo alarmExhibition iniciado. Esperando por evento..." << endl;

    hFile = CreateMailslot(mailAlarm, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
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
    cout << "HH:MM:SS NSEQ: ###### TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO PRI: ###" << endl;
    cout << "=============================================================" << endl;

    while (true) {
        WaitForSingleObject(hAlarmExhibitionEvent, INFINITE);
        bool successRead = ReadFile(hFile, &msg, 100, &bytesRead, NULL);
        if (!successRead) {
            cout << "Falha na leitura de mailsot de dados de otimização" << endl;
            exit(1);
        }

        cout << formatToExhibition(msg) << endl;
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

string formatToExhibition(string input) {
    string nSeq = input.substr(0, 6);
    string id = input.substr(11, 4);
    string priority = input.substr(15, 3);
    string time = input.substr(19, 8);
    string text = mapIdToText(id);

    stringstream result;
    result << time << " NSEQ: " << nSeq << " " << text << " PRI: " << priority;
    return result.str();
}

string mapIdToText(string ID) {
    int idInt = atoi(ID.c_str());
    string message = "";
    switch (idInt) {
    case 0: 
        message = "Pressao de vapor";
        break;
    case 1:
        message = "Temperatura de vapor";
        break;
    case 2:
        message = "Temperatura vapor DSH secundario";
        break;
    case 3:
        message = "Temperatura vapor DSH primario";
        break;
    case 4:
        message = "Pressao agua de alimentacao";
        break;
    case 5:
        message = "Nivel tubulacao de vapor";
        break;
    case 6:
        message = "Nivel tanque dissolvedor";
        break;
    case 7:
        message = "Vazao agua de alimentacao";
        break;
    case 8:
        message = "Emissao de CO";
        break;
    case 9:
        message = "Vapor DSH primario 2";
        break;
    default:
        break;
    }

    message = fillWithWhitespaces(message, 30);
    message = message.substr(0, 30);

    return message;
}

string fillWithWhitespaces(string str, int size) {
    if (str.size() >= size) return str;

    stringstream result;
    result << str;
    for (int i = str.size(); i < size; i++) {
        result << " ";
    }
    return result.str();
}