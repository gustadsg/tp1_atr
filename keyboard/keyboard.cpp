// keyboard.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1 

#include <iostream>
#include <string.h>
#include <windows.h>
#include <process.h>    // _beginthreadex() e _endthreadex()  
#include <locale.h>
#include <vector>
#include <conio.h>

#include "../Utils/constants.h"

constexpr auto ESC = 0x1B;
constexpr auto ERROR_STATUS = 0;

/*
* Criação de handles para lidar com eventos
*/
HANDLE hDataCommunicationEvent;
HANDLE hOtimizationRemovalEvent;
HANDLE hProcessRemovalEvent;
HANDLE hAlarmRemovalEvent;
HANDLE hOtimizationExhibitionEvent;
HANDLE hProcessExhibitionEvent;
HANDLE hAlarmExhibitionEvent;
HANDLE hClearAlarmConsoleEvent;
HANDLE hExitEvent;

HANDLE hMailSlotOtimizationReady;
HANDLE hMailSlotAlarmReady;
HANDLE hMailSlotProcessReady;

HANDLE hSemaphoreHardDisk;

/*
* Criação de handles para lidar com thread de teclado
*/
HANDLE hKeyboardThread;

/*
* Variáveis globais
*/
bool processesCreated = FALSE;
bool dataCommunicationActivated = TRUE;
bool alarmRemovalActivated = TRUE;
bool processRemovalActivated = TRUE;
bool otimizationRemovalActivated = TRUE;
bool otimizationExhibitionActivated = TRUE;
bool processExhibitionActivated = TRUE;
bool alarmExhibitionActivated = TRUE;


using namespace std;

unsigned __stdcall keyboardHandler(void*);
void printEventCreationStatus(HANDLE status, string eventName);
void fireAlarmByCharacter(char commandId);
void printPulse(string pulseName);
bool createProcesses();
bool createEvents();
bool setup();
bool createThreads();
void toggleEvent(HANDLE& eventHandle, bool& currentState, string eventName);
void SetAllEvents();
void closeAllHandles();
bool createSemaphores();

int main()
{
	bool setupSucceeded = setup();
	if (!setupSucceeded) {
		cout << "Falha ao iniciar ambiente" << endl;
		exit(1);
	}
	
	WaitForSingleObject(hKeyboardThread, INFINITE);
	
	closeAllHandles();
}

unsigned __stdcall keyboardHandler(void *) {
	cout << "Inicializando thread de leitura de teclado..." << endl;

	string input = "";

	while (true) {
		cout << "digite uma entrada: ";
		char input = _getch();
		fireAlarmByCharacter(input);
	}
}

bool setup() {
	setlocale(LC_ALL, "Portuguese");

	bool eventsCreated = createEvents();
	if (!eventsCreated) {
		cout << "Falha ao criar eventos" << endl;
	}

	// estado inicial tudo desbloqueado
	SetAllEvents();

	bool processesCreated = createProcesses();
	if (!processesCreated) {
		cout << "Falha ao criar processos" << endl;
	}
	
	bool threadsCreated = createThreads();
	if (!threadsCreated) {
		cout << "Falha ao criar threads" << endl;
	}

	bool semaphoresCreated = createSemaphores();
	if (!semaphoresCreated) {
		cout << "Falha ao criar semáforos" << endl;
	}

	bool setupSucceeded = eventsCreated && processesCreated && threadsCreated && semaphoresCreated;
	return setupSucceeded;
}

bool createThreads() {
	unsigned threadID;
	int i = 0;
	hKeyboardThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		keyboardHandler,
		&i,
		0,
		&threadID
	);

	if ((int)hKeyboardThread == ERROR_STATUS) {
		int errorCode = GetLastError();
		cout << "Falha ao iniciar thread de leitura de teclado. Erro ID: " << errorCode << endl;
		return FALSE;
	}

	return TRUE;
}

bool createEvents() {
	hDataCommunicationEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT(dataCommunication)  // object name
	);
	printEventCreationStatus(hDataCommunicationEvent, "Global\\dataCommunicationEvent");


	hOtimizationRemovalEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT(otimizationRemoval)  // object name
	);
	printEventCreationStatus(hOtimizationRemovalEvent, "otimizationRemovalEvent");
	
	hProcessRemovalEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT(processRemoval)  // object name
	);
	printEventCreationStatus(hProcessRemovalEvent, "processRemovalEvent");

	hAlarmRemovalEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT(alarmRemoval)  // object name
	);
	printEventCreationStatus(hAlarmRemovalEvent, "alarmRemovalEvent");

	hOtimizationExhibitionEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT(otimizationExhibition)  // object name
	);
	printEventCreationStatus(hOtimizationExhibitionEvent, "otimizationExhibitionEvent");

	hProcessExhibitionEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT(processExhibition)  // object name
	);
	printEventCreationStatus(hProcessExhibitionEvent, "processExhibitionEvent");

	hAlarmExhibitionEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT(alarmExhibition)  // object name
	);
	printEventCreationStatus(hAlarmExhibitionEvent, "alarmExhibitionEvent");

	hClearAlarmConsoleEvent = CreateEvent(
		NULL,               // default security attributes
		FALSE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT(clearAlarmConsole)  // object name
	);
	printEventCreationStatus(hClearAlarmConsoleEvent, "clearAlarmConsoleEvent");

	hExitEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT(exitAll)  // object name
	);
	printEventCreationStatus(hExitEvent, "exitEvent");

	hMailSlotOtimizationReady = CreateEvent(NULL, TRUE, FALSE, mailOtimizationReady);
	printEventCreationStatus(hMailSlotOtimizationReady, "mailSlotOtimizationReadyEvent");

	hMailSlotAlarmReady = CreateEvent(NULL, TRUE, FALSE, mailAlarmReady);
	printEventCreationStatus(hMailSlotAlarmReady, "mailSlotAlarmReadyEvent");

	hMailSlotProcessReady = CreateEvent(NULL, TRUE, FALSE, mailProcessReady);
	printEventCreationStatus(hMailSlotProcessReady, "mailSlotProcessReadyEvent");
	
	bool creationSucceeded = hDataCommunicationEvent && hOtimizationRemovalEvent && hProcessRemovalEvent && hAlarmRemovalEvent
		&& hOtimizationExhibitionEvent && hProcessExhibitionEvent && hAlarmExhibitionEvent && hClearAlarmConsoleEvent && hExitEvent;

	return creationSucceeded;
}

bool createProcesses() {
	// Não cria processos novamente se já tiver sido chamado anteriomente
	if (processesCreated) {
		cout << "Processos já foram criados. Ignorando comando..." << endl;
		return FALSE;
	}

	STARTUPINFO startupInfo;                    // StartUpInformation para novo processo
	PROCESS_INFORMATION processInfo;    // Informações sobre novo processo criado

	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	bool statusCircularMemory = CreateProcess(
		"..\\Debug\\circularMemory.exe",
		NULL,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		"..\\Debug",
		&startupInfo,
		&processInfo
	);
	if (!statusCircularMemory) {
		int errorCode = GetLastError();
		cout << "Erro na criaçao do processo dataCommunication. Erro ID: " << errorCode << endl;
	}

	bool statusDataOtimizationExhibition = CreateProcess(
		"..\\Debug\\otimizationDataExhibition.exe",
		NULL,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
		NULL,
		"..\\Debug",
		&startupInfo,
		&processInfo
	);
	if (!statusDataOtimizationExhibition) {
		int errorCode = GetLastError();
		cout << "Erro na criaçao do processo dataOtimizationExhibition. Erro ID: " << errorCode << endl;
	}

	bool statusAlarmsExhibition = CreateProcess(
		"..\\Debug\\alarmsExhibition.exe",
		NULL,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
		NULL,
		"..\\Debug",
		&startupInfo,
		&processInfo
	);
	if (!statusAlarmsExhibition) {
		int errorCode = GetLastError();
		cout << "Erro na criaçao do processo alarmsExhibition. Erro ID: " << errorCode << endl;
	}

	bool statusProcessDataExhibition = CreateProcess(
		"..\\Debug\\processDataExhibition.exe",
		NULL,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
		NULL,
		"..\\Debug",
		&startupInfo,
		&processInfo
	);
	if (!statusProcessDataExhibition) {
		int errorCode = GetLastError();
		cout << "Erro na criaçao do processo processDataExhibition. Erro ID: " << errorCode << endl;
	}

	processesCreated = TRUE;

	bool succeeded = statusDataOtimizationExhibition && statusCircularMemory;
	return succeeded;
};

void printEventCreationStatus(HANDLE status, string eventName) {
	if ((int) status == ERROR_STATUS) {
		int errorCode = GetLastError();
		cout << "Falha ao criar evento: " << eventName << ". Erro ID: " << errorCode << endl;
	}
	else {
		cout << "Evento " << eventName << " criado com sucesso!" << endl;
	}
}

void fireAlarmByCharacter(char commandId) {
	switch (commandId) {
	case 'c':
		toggleEvent(hDataCommunicationEvent, dataCommunicationActivated, "dataCommunicationEvent");
		break;
	case 'o':
		toggleEvent(hOtimizationRemovalEvent, otimizationRemovalActivated, "otimizationRemovalEvent");
		break;
	case 'p':
		toggleEvent(hProcessRemovalEvent, processRemovalActivated, "processRemovalEvent");
		break;
	case 'a':
		toggleEvent(hAlarmRemovalEvent, alarmRemovalActivated, "alarmRemovalEvent");
		break;
	case 't':
		toggleEvent(hOtimizationExhibitionEvent, otimizationExhibitionActivated, "otimizationExhibitionEvent");
		break;
	case 'r':
		toggleEvent(hProcessExhibitionEvent, processExhibitionActivated, "processExhibitionEvent");
		break;
	case 'l':
		toggleEvent(hAlarmExhibitionEvent, alarmExhibitionActivated, "alarmExhibitionEvent");
		break;
	case 'z':
		PulseEvent(hClearAlarmConsoleEvent);
		printPulse("clearAlarmConsoleEvent");
		break;
	case ESC:
	default:
		printPulse("exitEvent");
		PulseEvent(hExitEvent);
		ExitProcess(0);
		break;
	}
}

void printPulse(string eventName) {
	cout << "Disparando evento de " << eventName << endl;
}

void printSet(string eventName) {
	cout << "Ativando evento de " << eventName << endl;
}

void printReset(string eventName) {
	cout << "Desativando evento de " << eventName << endl;
}

void toggleEvent(HANDLE &eventHandle, bool &currentState, string eventName) {
	if (currentState) {
		ResetEvent(eventHandle);
		printReset(eventName);
		currentState = FALSE;
	}
	else {
		SetEvent(eventHandle);
		printSet(eventName);
		currentState = TRUE;
	}
}

void closeAllHandles() {
	vector<HANDLE> allHandles = {
		hDataCommunicationEvent,
		hOtimizationRemovalEvent,
		hProcessRemovalEvent,
		hAlarmRemovalEvent,
		hOtimizationExhibitionEvent,
		hProcessExhibitionEvent,
		hAlarmExhibitionEvent,
		hClearAlarmConsoleEvent,
		hExitEvent,
		hKeyboardThread,
	};

	for (unsigned i = 0; i < allHandles.size(); i++) {
		CloseHandle(allHandles[i]);
	}
}

void SetAllEvents() {
	vector<HANDLE> allHandles = {
		hDataCommunicationEvent,
		hOtimizationRemovalEvent,
		hProcessRemovalEvent,
		hAlarmRemovalEvent,
		hOtimizationExhibitionEvent,
		hProcessExhibitionEvent,
		hAlarmExhibitionEvent,
	};

	for (unsigned i = 0; i < allHandles.size(); i++) {
		SetEvent(allHandles[i]);
	}
}

bool createSemaphores() {
	hSemaphoreHardDisk = CreateSemaphore(NULL, 0, 200, hardDisksemaphore);
	return hSemaphoreHardDisk;
}