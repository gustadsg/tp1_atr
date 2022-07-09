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
const char* mailOtimization = "\\\\.\\mailslot\\otimizationData";
const char* mailProcess = "\\\\.\\mailslot\\processData";
const char* mailAlarm = "\\\\.\\mailslot\\alarm";

// mailslots events
const char* mailOtimizationReady = "Global\\otimizationMailslotEvent";
const char* mailAlarmReady = "Global\\alarmMailslotEvent";
const char* mailProcessReady = "Global\\processMailslotEvent";

#endif