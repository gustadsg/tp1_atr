#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

class MessageGenerator
{
public:
	string generateOtimizationSystemMessage(int position);
	string generateSCADAMessage(int position);
	string generateAlarmMessage(int position);

private:	
	string padStartNumber(float number, int desiredLength, string fillWith);
	string strRandomFormattedNumber(int length, int numDecimalPlaces, int maxNumber = 100);
	string getTimeString();
};

