#include "MessageGenerator.h"

string MessageGenerator::generateOtimizationSystemMessage(int position) {
    string nseq = this->padStartNumber(position, 6, "0");
    int type = 11;
    string press = this->strRandomFormattedNumber(6, 1, 99999);
    string temp = this->strRandomFormattedNumber(6, 1, 99999);
    string vol = this->strRandomFormattedNumber(5, 0, 99999);
    string time = getTimeString();
    

    string pipe = "|";
    

    stringstream result;
    result << nseq << pipe << type << pipe << press << pipe << temp << pipe << vol << pipe << time << endl;

    return result.str();
}

string MessageGenerator::generateSCADAMessage(int position) {
    string nseq = this->padStartNumber(position, 6, "0");
    int type = 22;
    string pressT = this->strRandomFormattedNumber(6, 1, 99999);
    string temp = this->strRandomFormattedNumber(6, 1, 99999);
    string pressG = this->strRandomFormattedNumber(6, 1, 99999);
    string level = this->strRandomFormattedNumber(6, 1, 99999);
    string time = getTimeString();


    string pipe = "|";


    stringstream result;
    result << nseq << pipe << type << pipe << pressT << pipe << temp << pipe << pressG << pipe << level << pipe << time << endl;

    return result.str();
}

string MessageGenerator::generateAlarmMessage(int position)
{
    string nseq = this->padStartNumber(position, 6, "0");
    int type = 55;
    string ID = this->strRandomFormattedNumber(4, 0, 9999);
    string priority = this->strRandomFormattedNumber(3, 0, 9999);
    string time = getTimeString();

    string pipe = "|";


    stringstream result;
    result << nseq << pipe << type << pipe << ID << pipe << priority << pipe << time << endl;

    return result.str();
}

string MessageGenerator::strRandomFormattedNumber(int length, int numDecimalPlaces, int maxNumber)
{
    float aux = (float)(rand() % maxNumber + 1);
    float res = aux / pow(10, numDecimalPlaces);
    
    return this->padStartNumber(res, length, "0");
}

string MessageGenerator::padStartNumber(float number, int desiredLength, string fillWith) {
    stringstream ss;
    ss << number;

    if (ss.str().length() >= desiredLength) {
        return ss.str();
    }

    stringstream res;
    int remainingSlots = desiredLength - ss.str().length();
    while (remainingSlots > 0) {
        res << fillWith;
        remainingSlots = desiredLength - (res.str().length() + ss.str().length());
    };

    res << ss.str();

    return res.str();
}

string MessageGenerator::getTimeString() {
    time_t t; time(&t);
    struct tm timeinfo;
    errno_t err = localtime_s(&timeinfo, &t);
    if (err) {
        cout << "Erro ao buscar hora atual" << endl;
    }

    string dots = ":";

    stringstream ssDate;
    ssDate << this->padStartNumber(timeinfo.tm_hour, 2, "0") << dots << this->padStartNumber(timeinfo.tm_min, 2, "0") << dots << this->padStartNumber(timeinfo.tm_sec, 2, "0");

    return ssDate.str();
}