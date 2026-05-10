#define _CRT_SECURE_NO_WARNINGS   //i have used to stop the localtime errors thrown by vs
#include "Validator.hpp"
#include <ctime>
bool Validator::validateContact(const char* contact) {
    if (getLength(contact) != 11) {
        return false;
    }
    for (int i = 0; contact[i] != '\0'; i++) {
        if (contact[i] < '0' || contact[i] > '9') {
            return false;
        }
    }
    return true;
}

bool Validator::validatePassword(const char* pass) {
    return getLength(pass) >= 6;
}

bool Validator::validatePositiveFloat(float val) {
    return val > 0.0f;
}

bool Validator::validateDate(const char* date) {
    if (getLength(date) != 10) {
        return false;
    }
    if (date[2] != '-' || date[5] != '-') {
        return false;
    }
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) {
            continue;
        }
        if (date[i] < '0' || date[i] > '9') {
            return false;
        }
    }
    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');
    if (day < 1 || day > 31) {
        return false;
    }
    if (month < 1 || month > 12) {
        return false;
    }
    time_t t = time(nullptr);
    tm* currTime = localtime(&t);          
    int currentYear = currTime->tm_year + 1900;
    int currentMonth = currTime->tm_mon + 1;
    int currentDay = currTime->tm_mday;
    if (year < currentYear) {
        return false;
    }
    if (year == currentYear && month < currentMonth) {
        return false;
    }
    if (year == currentYear && month == currentMonth && day < currentDay) {
        return false;
    }
    return true;
}

bool Validator::validateTimeSlot(const char* ts) {
    const char* validSlots[] = { "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00" };
    for (int i = 0; i < 8; i++) {
        if (isEqual(ts, validSlots[i])) {
            return true;
        }
    }
    return false;
}

bool Validator::validateID(int id) {
    return id > 0;
}

bool Validator::validateMenuChoice(int choice, int minChoice, int maxChoice) {
    return (choice >= minChoice && choice <= maxChoice);
}