#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP
#include <iostream>
#include <fstream>
#include "Storage.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Admin.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"
#include "helperFunctions.hpp"
#include "HospitalException.hpp"
using namespace std;

class File {
private:
public:
    void filePatients(Storage<Patient>& storPat);
    void fileDoctor(Storage<Doctor>& storDoc);
    void fileAppointments(Storage<Appointment>& storApp);
    void fileAdmins(Storage<Admin>& storAdmin);
    void fileBills(Storage<Bill>& storBill);
    void filePrescriptions(Storage<Prescription>& storPres);
    void savePatients(Storage<Patient>& storPat);
    void saveDoctor(Storage<Doctor>& storDoc);
    void saveAppointment(Storage<Appointment>& storApp);
    void saveBills(Storage<Bill>& storBill);
    void savePrescriptions(Storage<Prescription>& storPres);
    void appendAppointment(Appointment& a);
    void appendBill(Bill& b);
    void appendPrescription(Prescription& p);
    void appendDoctor(Doctor& d);
    void writeSecurityLog(const char* role, int enteredID, const char* result);
    void readSecurityLog();
    void dischargePatient(int patientID, Storage<Patient>& storPat, Storage<Appointment>& storApp,
        Storage<Bill>& storBill, Storage<Prescription>& storPres);
    void loadAllFiles(Storage<Patient>& p, Storage<Doctor>& d, Storage<Appointment>& a, Storage<Admin>& adm,
        Storage<Bill>& b, Storage<Prescription>& pr);
    void saveAllFiles(Storage<Patient>& p, Storage<Doctor>& d, Storage<Appointment>& a, Storage<Bill>& b,
        Storage<Prescription>& pr);
};

#endif