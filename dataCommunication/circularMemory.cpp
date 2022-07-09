// circularMemoryHandler.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//
#define _CRT_SECURE_NO_WARNINGS_GLOBALS
#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include <time.h>
#include <sstream>
#include <process.h>    // _beginthreadex() e _endthreadex()  

#include "../Utils/constants.h"
#include "../Utils/Message.h"
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

HANDLE hFileOtimization;
HANDLE hMailProcess;
HANDLE hMailAlarm;

HANDLE hSemaphoreHardDisk;

/*
* Variáveis globais
*/
vector<string> circularMemory;
MessageGenerator messageGenerator = MessageGenerator();
long int numSeq = 0;
long int numSeqOtimization = 0;
long int numSeqAlarm = 0;
long int numSeqProcess = 0;

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
void removeMessageFromMemory(int typeToRemove, HANDLE mailSlot);
void removeOtimizationMessageFromMemory();
int getMessageType(string message);
void printCircularMemory();
bool createTimers();
void printColorfulMessage(string message, int type);
bool createMailSlotsFiles();
bool openSemaphores();

void InfiniteWaitForSingleObject(HANDLE handle);
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

    bool timeSlotsFilesCreated = createMailSlotsFiles();
    if (!timeSlotsFilesCreated) {
        cout << "Erro na criação de timeSlotsFiles" << endl;
        exit(1);
    }

    bool threadsCreated = createThreads();
    if (!threadsCreated) {
        cout << "Erro na criação de threads" << endl;
        exit(1);
    }

    bool semaphoresOpened = openSemaphores();
    if (!threadsCreated) {
        cout << "Erro na criação de semáforos" << endl;
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
        InfiniteWaitForSingleObject(hDataCommunicationEvent);

        WaitForSingleObject(hTimerProcessData, INFINITE);

        string scadaMessage = messageGenerator.generateSCADAMessage(numSeqProcess);
        addMessageToMemory(scadaMessage);
        printColorfulMessage(scadaMessage, 22);
        numSeqProcess++;
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
        InfiniteWaitForSingleObject(hDataCommunicationEvent);

        WaitForSingleObject(hTimerAlarm, INFINITE);
        
        string alarmMessage = messageGenerator.generateAlarmMessage(numSeqAlarm);
        addMessageToMemory(alarmMessage);
        printColorfulMessage(alarmMessage, 55);
        numSeqAlarm++;

        LARGE_INTEGER Preset;
        int msToActivate = 1000 + rand() % 4000;
        int timeToActivateInNsPackages = -(msToActivate * msMultFactor);
        Preset.QuadPart = timeToActivateInNsPackages;

        bool timerSet = SetWaitableTimer(hTimerAlarm, &Preset, msToActivate, NULL, NULL, FALSE);
        if (!timerSet) {
            int errorCode = GetLastError();
            cout << "Erro ao resetar timer de alarme. Erro código" << errorCode << endl;
            exit(1);
        }
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
        InfiniteWaitForSingleObject(hDataCommunicationEvent);

        WaitForSingleObject(hTimerOtimizationData, INFINITE);

        string otimizationSystemMessage = messageGenerator.generateOtimizationSystemMessage(numSeqOtimization);
        
        addMessageToMemory(otimizationSystemMessage);
        printColorfulMessage(otimizationSystemMessage, 11);
        numSeqOtimization++;

        LARGE_INTEGER Preset;
        int msToActivate = 1000 + rand() % 4000;
        int timeToActivateInNsPackages = - (msToActivate * msMultFactor);
        Preset.QuadPart = timeToActivateInNsPackages;

        bool timerSet = SetWaitableTimer(hTimerOtimizationData, &Preset, msToActivate, NULL, NULL, FALSE);
        if (!timerSet) {
            int errorCode = GetLastError();
            cout << "Erro ao resetar timer de alarme. Erro código" << errorCode << endl;
            exit(1);
        }
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
        InfiniteWaitForSingleObject(hAlarmRemovalEvent);
        int codeAlarms = 55;
        removeMessageFromMemory(codeAlarms, hMailAlarm);
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
        InfiniteWaitForSingleObject(hProcessDataRemovalEvent);
        int codeProcessData = 22;
        removeMessageFromMemory(codeProcessData, hMailProcess);
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
        InfiniteWaitForSingleObject(hOtimizationDataRemovalEvent);
        int codeOtimizationData = 11;
        removeOtimizationMessageFromMemory();
        
    }
}

unsigned __stdcall threadExit(void*) {
    hExitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, exitAll);
    WaitForSingleObject(hExitEvent, INFINITE);
    ExitProcess(0);
}

void addMessageToMemory(string message) {
    InfiniteWaitForSingleObject(hCircularMemoryMutex);
    bool isFull = !(circularMemory.size() < sizeMemory);
    if (isFull) {
        cout << "A memória circular está cheia. Bloqueando thread..." << endl;
        ReleaseMutex(hCircularMemoryMutex);

        InfiniteWaitForSingleObject(hSemaphoreListFull); // Espera o semaforo encher novamente. Efeito colateral: consome uma posicao sem adicionar conteudo
        LogReleaseSemaphore(hSemaphoreListFull); // libera a posicao pois nao adicionou conteudo
        cout << "Espaço liberado na memória circular. Desbloqueando thread..." << endl;
    }
    else {
        cout << "Conquistando semaforo de memoria circular" << endl;
        InfiniteWaitForSingleObject(hSemaphoreListFull);
        circularMemory.push_back(message);
        numSeq++;
        cout << "Mensagem adicionada à memória. Contagem atual: " << circularMemory.size() << endl;
        ReleaseMutex(hCircularMemoryMutex);
    }
}

void removeMessageFromMemory(int typeToRemove, HANDLE hFileDescriptor) {
    InfiniteWaitForSingleObject(hCircularMemoryMutex);

    for (int i = 0; i < circularMemory.size(); i++) {
        int typeOfMessage = getMessageType(circularMemory[i]);

        if (typeOfMessage == typeToRemove) {
            char msg[100];
            strcpy_s(msg, circularMemory[i].c_str());
            msg[circularMemory[i].size()] = '\0';
            DWORD bytesWritten = 0;
    
            bool success = WriteFile(hFileDescriptor, &msg, sizeof(msg), &bytesWritten, NULL);
            if (!success) {
                int error = GetLastError();
                cout << "Erro ao escrever em mailSlot ou arquivo. Erro código: " << error << endl;
            }
            cout << bytesWritten << " bytes escritos em arquivo ou mailsot" << endl;
            cout << "Removendo mensagem do tipo " << typeOfMessage << ". Mensagem: " << msg << endl;
            circularMemory.erase(next(circularMemory.begin(), i));
            LogReleaseSemaphore(hSemaphoreListFull);
        }
    }
    ReleaseMutex(hCircularMemoryMutex);
}

void removeOtimizationMessageFromMemory() {
    InfiniteWaitForSingleObject(hCircularMemoryMutex);
    long currentPosition = 0L;
    long lastPosition = 0L;

    for (int i = 0; i < circularMemory.size(); i++) {
        int typeOfMessage = getMessageType(circularMemory[i]);
        int otimizationType = 11;

        if (typeOfMessage == otimizationType) {
            char msg[otimizationMsgSize];
            strcpy_s(msg, otimizationMsgSize, circularMemory[i].c_str());
            DWORD bytesWritten = 0;

            bool success = WriteFile(hFileOtimization, &msg, sizeof(msg), &bytesWritten, NULL);
            if (!success) {
                int error = GetLastError();
                cout << "Erro ao escrever em mailSlot ou arquivo. Erro código: " << error << endl;
            }
            cout << bytesWritten << " bytes escritos em arquivo ou mailsot" << endl;
            cout << "Removendo mensagem do tipo " << typeOfMessage << ". Mensagem: " << msg << endl;
            circularMemory.erase(next(circularMemory.begin(), i));

            if ((numSeqOtimization % maxFileRows) == 0) {
                SetFilePointer(hFileOtimization, 0, NULL, FILE_BEGIN);
            }
            LogReleaseSemaphore(hSemaphoreListFull);
            LogReleaseSemaphore(hSemaphoreHardDisk);
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
    DWORD color = FOREGROUND_INTENSITY;
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
    int msToActivate = 1000 + rand() % 4000;
    int timeToActivateInNsPackages = (msToActivate * msMultFactor);
    Preset.QuadPart = timeToActivateInNsPackages;

    timerSet = SetWaitableTimer(hTimerAlarm, &Preset, msToActivate, NULL, NULL, FALSE);
    if (!hTimerAlarm || !timerSet) return FALSE;

    // dados de processo
    hTimerProcessData = CreateWaitableTimer(NULL, FALSE, timerProcessData);
    msToActivate = 500;
    timeToActivateInNsPackages = (msToActivate * msMultFactor);
    Preset.QuadPart = timeToActivateInNsPackages;

    timerSet = SetWaitableTimer(hTimerProcessData, &Preset, msToActivate, NULL, NULL, FALSE);
    if (!hTimerProcessData || !timerSet) return FALSE;

    // dados de otimização
    hTimerOtimizationData = CreateWaitableTimer(NULL, FALSE, timerOtimizationData);
    msToActivate = 1000 + rand() % 4000;
    timeToActivateInNsPackages = (msToActivate * msMultFactor);
    Preset.QuadPart = timeToActivateInNsPackages;

    timerSet = SetWaitableTimer(hTimerOtimizationData, &Preset, msToActivate, NULL, NULL, FALSE);
    if (!hTimerOtimizationData || !timerSet) return FALSE;

    return TRUE;
}

bool createMailSlotsFiles() {
    hFileOtimization = CreateFile(
        fileOtimization,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (!hFileOtimization) {
        int errorCode = GetLastError();
        cout << "Erro ao criar arquivo de disco de dados de otimização" << errorCode << endl;
    }
    cout << "Criado arquivo de leitura de dados de otimização" << endl;
    
    HANDLE hProcessReady = OpenEvent(EVENT_ALL_ACCESS, FALSE, mailProcessReady);
    if (!hProcessReady) {
        cout << "Erro ao abrir evento de mailslot de dados de processo" << endl;
        return false;
    }
    WaitForSingleObject(hProcessReady, INFINITE);
    hMailProcess= CreateFile(
        mailProcess,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (!hMailProcess) {
        int errorCode = GetLastError();
        cout << "Erro ao criar arquivo a partir de mailSlot mailProcess" << errorCode << endl;
    }
    cout << "Criado arquivo de leitura de dados de processo" << endl;
    
    HANDLE hAlarmReady = OpenEvent(EVENT_ALL_ACCESS, FALSE, mailAlarmReady);
    if (!hAlarmReady) {
        cout << "Erro ao abrir evento de mailslot de alarmes" << endl;
        return false;
    }
    WaitForSingleObject(hAlarmReady, INFINITE);
    hMailAlarm = CreateFile(
        mailAlarm,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (!hMailAlarm) {
        int errorCode = GetLastError();
        cout << "Erro ao criar arquivo a partir de mailSlot mailAlarm" << errorCode << endl;
    }
    cout << "Criado arquivo de leitura de alarmes" << endl;
    return hFileOtimization && hMailAlarm && hMailProcess;
}

bool openSemaphores() {
    hSemaphoreHardDisk = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, hardDisksemaphore);
    return hSemaphoreHardDisk;
}

void InfiniteWaitForSingleObject(HANDLE handle) {
    WaitForSingleObject(handle, INFINITE);
}

void LogReleaseSemaphore(HANDLE handle) {
    InfiniteWaitForSingleObject(hMutexPrint);
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