#ifndef PRESCRIPTION_HPP
#define PRESCRIPTION_HPP
#include <iostream>
#include "helperFunctions.hpp"
using namespace std;
class Prescription {
private:
    int prescriptionID;
    int appointmentID;
    int patientID;
    int doctorID;
    char* date;
    char* medicines;
    char* notes;
public:
    Prescription();
    Prescription(int pID, int appID, int patID, int docID, const char* d, const char* meds, const char* n);
    ~Prescription();
    Prescription(const Prescription& p);
    Prescription& operator=(const Prescription& p);
    int getID();
    int getAppointmentID();
    int getPatientID();
    int getDoctorID();
    const char* getDate();
    const char* getMedicines();
    const char* getNotes();
    friend ostream& operator<<(ostream& out, const Prescription& p);
};

#endif