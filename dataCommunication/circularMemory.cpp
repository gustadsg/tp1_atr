// circularMemoryHandler.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include <time.h>
#include <sstream>
#include <process.h>    // _beginthreadex() e _endthreadex()  

#include "../Utils/constants.h"
#include "MessageGenerator.h"

using namespace std;

/*
* Criação de handle de mutex para cuidar de exclusão mútua
*/
HANDLE hCircularMemoryMutex;
HANDLE hSemaphoreListFull;

/*
* Criação de handles de eventos e threads
*/
HANDLE hThreadDataCommunicationAlarm;
HANDLE hThreadDataCommunicationProcess;
HANDLE hThreadDataCommunicationOtimization;
HANDLE hThreadRemoveAlarms;
HANDLE hThreadRemoveOtimizationData;
HANDLE hThreadRemoveProcessData;
HANDLE hThreadExit;

HANDLE hDataCommunicationEvent;
HANDLE hAlarmRemovalEvent;
HANDLE hProcessDataRemovalEvent;
HANDLE hOtimizationDataRemovalEvent;
HANDLE hExitEvent;

HANDLE hMutexPrint;

HANDLE hTimerAlarm;
HANDLE hTimerProcessData;
HANDLE hTimerOtimizationData;


/*
* Variáveis globais
*/
vector<string> circularMemory;
MessageGenerator messageGenerator = MessageGenerator();
long int numSeq = 0;

/*
* Constantes
*/
constexpr int sizeMemory = 100;
const char * timerAlarms = "timerAlarms";
const char * timerProcessData = "timerProcessData";
const char * timerOtimizationData = "timerOtimizationData";
const int msMultFactor = 10000;

bool createSynchronizationObjects();
bool createThreads();
unsigned __stdcall threadDataCommunicationProcess(void*);
unsigned __stdcall threadDataCommunicationAlarm(void*);
unsigned __stdcall threadDataCommunicationOtimization(void*);
unsigned __stdcall threadRemoveAlarms(void*);
unsigned __stdcall threadRemoveProcessData(void*);
unsigned __stdcall threadRemoveOtimizationData(void*);
unsigned __stdcall threadExit(void*);
void addMessageToMemory(string message);
void removeMessageFromMemory(int typeToRemove);
int getMessageType(string message);
void printCircularMemory();
bool createTimers();
void printColorfulMessage(string message, int type);

void LogWaitForSingleObject(HANDLE handle);
void LogReleaseSemaphore(HANDLE handle);

int main()
{
    cout << "Processo dataCommunication iniciado. Esperando por evento..." << endl;

    bool mutexesCreated = createSynchronizationObjects();
    if (!mutexesCreated) {
        cout << "Erro na criação de mutexes" << endl;
        exit(1);
    }

    bool timersCreated = createTimers();
    if (!timersCreated) {
        cout << "Erro na criação de timers" << endl;
        exit(1);
    }

    bool threadsCreated = createThreads();
    if (!threadsCreated) {
        cout << "Erro na criação de threads" << endl;
        exit(1);
    }

    HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(cout_handle, FOREGROUND_INTENSITY);

    HANDLE threads[5] = { hThreadDataCommunicationAlarm , hThreadRemoveAlarms , hThreadRemoveProcessData, hThreadRemoveOtimizationData, hThreadExit };

    WaitForMultipleObjects(5, threads, TRUE, INFINITE);

    return 0;
}

bool createSynchronizationObjects() {
    hCircularMemoryMutex = CreateMutex(NULL, FALSE, "circularMemoryMutex");
    if (!hCircularMemoryMutex) {
        int errorCode = GetLastError();
        cout << "Erro ao criar Mutex de memória circular. Erro ID: " << errorCode << endl;
        return FALSE;
    }

    hSemaphoreListFull = CreateSemaphore(NULL, sizeMemory, sizeMemory, "semaforoLista");
    if (!hSemaphoreListFull) {
        int errorCode = GetLastError();
        cout << "Erro ao criar semáforo de lista cheia. Erro ID: " << errorCode << endl;
        return FALSE;
    }

    hMutexPrint = CreateMutex(NULL, FALSE, "MutexPrint");
    if (!hMutexPrint) {
        int errorCode = GetLastError();
        cout << "Erro ao criar Mutex de print. Erro ID: " << errorCode << endl;
        return FALSE;
    }

    return TRUE;
}

bool createThreads() {
    hThreadDataCommunicationAlarm = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadDataCommunicationAlarm,
        NULL,
        0,
        NULL
    );
    if (!hThreadDataCommunicationAlarm) {
        int errorCode = GetLastError();
        cout << "Erro ao criar thread de adição de alarmes. Erro ID: " << errorCode << endl;
        return FALSE;
    }
    
    hThreadDataCommunicationProcess = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadDataCommunicationProcess,
        NULL,
        0,
        NULL
    );
    if (!hThreadDataCommunicationProcess) {
        int errorCode = GetLastError();
        cout << "Erro ao criar thread de adição de dados de processos. Erro ID: " << errorCode << endl;
        return FALSE;
    }

    hThreadDataCommunicationOtimization = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadDataCommunicationOtimization,
        NULL,
        0,
        NULL
    );
    if (!hThreadDataCommunicationOtimization) {
        int errorCode = GetLastError();
        cout << "Erro ao criar thread de adição de dados de otimização. Erro ID: " << errorCode << endl;
        return FALSE;
    }

    hThreadRemoveProcessData = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadRemoveProcessData,
        NULL,
        0,
        NULL
    );
    if (!hThreadRemoveProcessData) {
        int errorCode = GetLastError();
        cout << "Erro ao criar thread de remoção de dados de processo. Erro ID: " << errorCode << endl;
        return FALSE;
    }

    hThreadRemoveAlarms = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadRemoveAlarms,
        NULL,
        0,
        NULL
    );
    if (!hThreadRemoveAlarms) {
        int errorCode = GetLastError();
        cout << "Erro ao criar thread de remoção de alarmes. Erro ID: " << errorCode << endl;
        return FALSE;
    }
    
    hThreadRemoveOtimizationData = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadRemoveOtimizationData,
        NULL,
        0,
        NULL
    );
    if (!hThreadRemoveOtimizationData) {
        int errorCode = GetLastError();
        cout << "Erro ao criar thread de remoção de dados de otimização. Erro ID: " << errorCode << endl;
        return FALSE;
    }
    
    hThreadExit = (HANDLE)_beginthreadex(
        NULL,
        0,
        threadExit,
        NULL,
        0,
        NULL
    );
    if (!hThreadExit) {
        int errorCode = GetLastError();
        cout << "Erro ao criar thread de detecção de comando de exit. Erro ID: " << errorCode << endl;
        return FALSE;
    }

    return hThreadRemoveAlarms && hThreadRemoveOtimizationData && hThreadRemoveProcessData && hThreadExit;
}

unsigned __stdcall threadDataCommunicationProcess(void*) {
    cout << "Inicializando thread de adição de mensagens..." << endl;
    hDataCommunicationEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, dataCommunication);
    if (!hDataCommunicationEvent) {
        int errorCode = GetLastError();
        cout << "Erro na abertura de evento de comunicação de dados. Erro ID: " << errorCode << endl;
        exit(1);
    }

    while (true) {
        LogWaitForSingleObject(hDataCommunicationEvent);

        WaitForSingleObject(hTimerProcessData, INFINITE);

        cout << "22" << endl;
        string scadaMessage = messageGenerator.generateSCADAMessage(numSeq);
        addMessageToMemory(scadaMessage);
        printColorfulMessage(scadaMessage, 22);
    }
}

unsigned __stdcall threadDataCommunicationAlarm(void*) {
    cout << "Inicializando thread de adição de mensagens..." << endl;
    hDataCommunicationEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, dataCommunication);
    if (!hDataCommunicationEvent) {
        int errorCode = GetLastError();
        cout << "Erro na abertura de evento de comunicação de dados. Erro ID: " << errorCode << endl;
        exit(1);
    }

    while (true) {
        LogWaitForSingleObject(hDataCommunicationEvent);

        WaitForSingleObject(hTimerAlarm, INFINITE);
        LARGE_INTEGER Preset;
        int msToActivate = 1000 + rand() % 4000;
        int timeToActivateInNsPackages = (msToActivate * msMultFactor);
        Preset.QuadPart = timeToActivateInNsPackages;

        bool timerSet = SetWaitableTimer(hTimerAlarm, &Preset, NULL, NULL, NULL, FALSE);

        string alarmMessage = messageGenerator.generateAlarmMessage(numSeq);
        cout << "55" << endl;
        addMessageToMemory(alarmMessage);
        printColorfulMessage(alarmMessage, 55);
    }
}

unsigned __stdcall threadDataCommunicationOtimization(void*) {
    cout << "Inicializando thread de adição de mensagens..." << endl;
    hDataCommunicationEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, dataCommunication);
    if (!hDataCommunicationEvent) {
        int errorCode = GetLastError();
        cout << "Erro na abertura de evento de comunicação de dados. Erro ID: " << errorCode << endl;
        exit(1);
    }

    while (true) {
        LogWaitForSingleObject(hDataCommunicationEvent);

        WaitForSingleObject(hTimerAlarm, INFINITE);

        string otimizationSystemMessage = messageGenerator.generateOtimizationSystemMessage(numSeq);
        cout << "11" << endl;
        addMessageToMemory(otimizationSystemMessage);
        printColorfulMessage(otimizationSystemMessage, 11);
    }
}

unsigned __stdcall threadRemoveAlarms(void*) {
    cout << "Inicializando thread de remoção de alarmes..." << endl;
    hAlarmRemovalEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, alarmRemoval);
    if (!hAlarmRemovalEvent) {
        int errorCode = GetLastError();
        cout << "Erro na abertura de evento de remoção de alarmes. Erro ID: " << errorCode << endl;
        exit(1);
    }

    while (true) {
        LogWaitForSingleObject(hAlarmRemovalEvent);
        int codeAlarms = 55;
        removeMessageFromMemory(codeAlarms);
    }
}

unsigned __stdcall threadRemoveProcessData(void*) {
    cout << "Inicializando thread de remoção de dados de processo..." << endl;
    hProcessDataRemovalEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, processRemoval);
    if (!hProcessDataRemovalEvent) {
        int errorCode = GetLastError();
        cout << "Erro na abertura de evento de dados de processo. Erro ID: " << errorCode << endl;
        exit(1);
    }

    while (true) {
        LogWaitForSingleObject(hProcessDataRemovalEvent);
        int codeProcessData = 22;
        removeMessageFromMemory(codeProcessData);
    }
}

unsigned __stdcall threadRemoveOtimizationData(void*) {
    cout << "Inicializando thread de remoção de dados de otimização..." << endl;
    hOtimizationDataRemovalEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, otimizationRemoval);
    if (!hOtimizationDataRemovalEvent) {
        int errorCode = GetLastError();
        cout << "Erro na abertura de evento de remoção dados de otimização. Erro ID: " << errorCode << endl;
        exit(1);
    }

    while (true) {
        LogWaitForSingleObject(hOtimizationDataRemovalEvent);
        int codeOtimizationData = 11;
        removeMessageFromMemory(codeOtimizationData);
    }
}

unsigned __stdcall threadExit(void*) {
    hExitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, exitAll);
    WaitForSingleObject(hExitEvent, INFINITE);
    ExitProcess(0);
}

void addMessageToMemory(string message) {
    LogWaitForSingleObject(hCircularMemoryMutex);
    bool isFull = !(circularMemory.size() < sizeMemory);
    if (isFull) {
        cout << "A memória circular está cheia. Bloqueando thread..." << endl;
        ReleaseMutex(hCircularMemoryMutex);

        LogWaitForSingleObject(hSemaphoreListFull); // Espera o semaforo encher novamente. Efeito colateral: consome uma posicao sem adicionar conteudo
        LogReleaseSemaphore(hSemaphoreListFull); // libera a posicao pois nao adicionou conteudo
        cout << "Espaço liberado na memória circular. Desbloqueando thread..." << endl;
    }
    else {
        cout << "Conquistando semaforo de memoria circular" << endl;
        LogWaitForSingleObject(hSemaphoreListFull);
        circularMemory.push_back(message);
        numSeq++;
        cout << "Mensagem adicionada à memória. Contagem atual: " << circularMemory.size() << endl;
        ReleaseMutex(hCircularMemoryMutex);
    }
}

void removeMessageFromMemory(int typeToRemove) {
    LogWaitForSingleObject(hCircularMemoryMutex);

    for (int i = 0; i < circularMemory.size(); i++) {
        int typeOfMessage = getMessageType(circularMemory[i]);

        if (typeOfMessage == typeToRemove) {
            cout << "Removendo mensagem do tipo " << typeOfMessage << ". Mensagem: " << circularMemory[i] << endl;
            circularMemory.erase(next(circularMemory.begin(), i));
            LogReleaseSemaphore(hSemaphoreListFull);
        }
    }
    ReleaseMutex(hCircularMemoryMutex);
}

int getMessageType(string message) {
    string strType = message.substr(7, 9);
    
    stringstream ss;
    ss << strType;
    
    int intType = 0;
    ss >> intType;

    return intType;
}

void printColorfulMessage(string message, int type){
    WaitForSingleObject(hMutexPrint, INFINITE);
    DWORD color;
    if (type == 11) color = FOREGROUND_RED;
    if (type == 22) color = FOREGROUND_GREEN;
    if (type == 55) color = FOREGROUND_BLUE;

    HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(cout_handle, color);
    cout << message;
    SetConsoleTextAttribute(cout_handle, FOREGROUND_INTENSITY);
    ReleaseMutex(hMutexPrint);
}

void printCircularMemory() {
    for (int i = 0; i < circularMemory.size(); i++) {
        cout << circularMemory[i];
    }
}

bool createTimers() {
    bool timerSet = FALSE;

    // alarmes
    hTimerAlarm = CreateWaitableTimer(NULL, FALSE, timerAlarms);
    LARGE_INTEGER Preset;
    int msToActivate = 5000;
    int timeToActivateInNsPackages = (msToActivate * msMultFactor);
    Preset.QuadPart = timeToActivateInNsPackages;

    timerSet = SetWaitableTimer(hTimerAlarm, &Preset, NULL, NULL, NULL, FALSE);
    if (!hTimerAlarm || !timerSet) return FALSE;

    // dados de processo
    hTimerProcessData = CreateWaitableTimer(NULL, FALSE, timerProcessData);
    msToActivate = 5000;
    timeToActivateInNsPackages = (msToActivate * msMultFactor);
    Preset.QuadPart = timeToActivateInNsPackages;

    timerSet = SetWaitableTimer(hTimerProcessData, &Preset, msToActivate, NULL, NULL, FALSE);
    if (!hTimerProcessData || !timerSet) return FALSE;

    // dados de otimização
    hTimerOtimizationData = CreateWaitableTimer(NULL, FALSE, timerOtimizationData);
    msToActivate = 5000;
    timeToActivateInNsPackages = (msToActivate * msMultFactor);
    Preset.QuadPart = timeToActivateInNsPackages;

    timerSet = SetWaitableTimer(hTimerOtimizationData, &Preset, msToActivate, NULL, NULL, FALSE);
    if (!hTimerOtimizationData || !timerSet) return FALSE;

    return TRUE;
}

void LogWaitForSingleObject(HANDLE handle) {
    int status = 0;
    WaitForSingleObject(handle, INFINITE);
}

void LogReleaseSemaphore(HANDLE handle) {
    LogWaitForSingleObject(hMutexPrint);
    int status = 0;
    LONG prevVal = 0;
    status = ReleaseSemaphore(handle, 1, &prevVal);
    if (!status) {
        int errorCode = GetLastError();
        cout << "Erro ao liberar semáforo. Erro ID: " << errorCode << endl;
    }   else {
        cout << "Semaforo incrementado. Valor atual: " << (prevVal + 1) << endl;
    }
    ReleaseMutex(hMutexPrint);
}