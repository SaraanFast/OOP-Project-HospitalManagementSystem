#include "Bill.hpp"
Bill::Bill() {
    billID = 0;
    patientID = 0;
    appointmentID = 0;
    amount = 0.0f;
    status = new char[8];
    copyStr(status, "unpaid");
    date = new char[2];
    copyStr(date, "");
}

Bill::Bill(int bID, int pID, int appID, float amt, const char* st, const char* d) {
    billID = bID;
    patientID = pID;
    appointmentID = appID;
    amount = amt;
    status = new char[getLength(st) + 1];
    copyStr(status, st);
    date = new char[getLength(d) + 1];
    copyStr(date, d);
}

Bill::Bill(const Bill& b) {
    billID = b.billID;
    patientID = b.patientID;
    appointmentID = b.appointmentID;
    amount = b.amount;
    status = new char[getLength(b.status) + 1];
    copyStr(status, b.status);
    date = new char[getLength(b.date) + 1];
    copyStr(date, b.date);
}

Bill& Bill::operator=(const Bill& b) {
    if (this != &b) {
        delete[] status;
        delete[] date;
        billID = b.billID;
        patientID = b.patientID;
        appointmentID = b.appointmentID;
        amount = b.amount;
        status = new char[getLength(b.status) + 1];
        copyStr(status, b.status);
        date = new char[getLength(b.date) + 1];
        copyStr(date, b.date);
    }
    return *this;
}

Bill::~Bill() {
    delete[] status;
    delete[] date;
}

int Bill::getID() {
    return billID;
}

int Bill::getPatientID() {
    return patientID;
}

int Bill::getAppointmentID() {
    return appointmentID;
}

float Bill::getAmount() {
    return amount;
}

const char* Bill::getStatus() {
    return status;
}

const char* Bill::getDate() {
    return date;
}

void Bill::setStatus(const char* st) {
    delete[] status;
    status = new char[getLength(st) + 1];
    copyStr(status, st);
}

ostream& operator<<(ostream& out, const Bill& b) {
    out << "Bill ID: " << b.billID<< " | Patient ID: " << b.patientID<< " | Appointment ID: " << b.appointmentID
        << " | Amount: PKR " << b.amount<< " | Status: " << b.status<< " | Date: " << b.date;
    return out;
}