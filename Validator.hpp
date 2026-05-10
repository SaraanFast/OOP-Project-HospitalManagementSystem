#ifndef VALIDATOR_H
#define VALIDATOR_H
#include "helperFunctions.hpp"
#include<ctime>
class Validator {
public:
    static bool validateContact(const char* contact);
    static bool validatePassword(const char* pass);
    static bool validatePositiveFloat(float val);
    static bool validateDate(const char* date);
    static bool validateTimeSlot(const char* ts);
    static bool validateID(int id);
    static bool validateMenuChoice(int choice, int minChoice, int maxChoice);
};

#endif