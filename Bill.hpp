#ifndef BILL_HPP
#define BILL_HPP
#include <iostream>
#include "helperFunctions.hpp"
using namespace std;
class Bill {
private:
    int billID;
    int patientID;
    int appointmentID;
    float amount;
    char* status;
    char* date;
public:
    Bill();
    Bill(int bID, int pID, int appID, float amt, const char* st, const char* d);
    Bill(const Bill& b);
    Bill& operator=(const Bill& b);
    ~Bill();
    int getID();
    int getPatientID();
    int getAppointmentID();
    float getAmount();
    const char* getStatus();
    const char* getDate();
    void setStatus(const char* st);
    friend ostream& operator<<(ostream& out, const Bill& b);
};

#endif