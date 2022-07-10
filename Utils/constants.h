#pragma once
#ifndef CONSTANTS

// eventos
const char* dataCommunication = "Global\\dataCommunicationEvent";
// remoção
const char* otimizationRemoval = "Global\\otimizationRemovalEvent";
const char* processRemoval = "Global\\processRemovalEvent";
const char* alarmRemoval = "Global\\alarmRemovalEvent";
// exibição
const char* otimizationExhibition = "Global\\otimizationExhibitionEvent";
const char* processExhibition = "Global\\processExhibitionEvent";
const char* alarmExhibition = "Global\\alarmExhibitionEvent";
// outros
const char* clearAlarmConsole = "Global\\clearAlarmConsoleEvent";
const char* exitAll = "Global\\exitEvent";

// mailslots
const char* mailProcess = "\\\\.\\mailslot\\processData";
const char* mailAlarm = "\\\\.\\mailslot\\alarm";

// mailslots events
const char* mailOtimizationReady = "Global\\otimizationMailslotEvent";
const char* mailAlarmReady = "Global\\alarmMailslotEvent";
const char* mailProcessReady = "Global\\processMailslotEvent";

// arquivos
const char* dirOtimization = "..\\dataOtimization";
const char* fileOtimization = "..\\dataOtimization\\data.txt";

// semaforos 
const char* hardDisksemaphore = "Global\\hardDiskSemaphore";

// contantes numericas
const int otimizationMsgSize = 39;
const int maxFileMessages = 3;
#endif