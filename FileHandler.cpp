#define _CRT_SECURE_NO_WARNINGS   //i have used to stop the localtime errors thrown by vs
#include "FileHandler.hpp"
#include <fstream>
#include <ctime>
void File::filePatients(Storage<Patient>& storPat) {
    ifstream f("patients.txt");
    if (!f.is_open()) {
        throw FileNotFoundException("Error! patients.txt could not be opened.");
    }
    char line[500];
    while (f.getline(line, 500)) {
        int index = 0;
        char temp[100];
        readFile(line, index, temp);
        int id = stringToFloat(temp);
        readFile(line, index, temp);
        char* n = new char[getLength(temp) + 1];
        copyStr(n, temp);
        readFile(line, index, temp);
        int a = stringToFloat(temp);
        readFile(line, index, temp);
        char g = temp[0];
        readFile(line, index, temp);
        char* cont = new char[getLength(temp) + 1];
        copyStr(cont, temp);
        readFile(line, index, temp);
        char* pass = new char[getLength(temp) + 1];
        copyStr(pass, temp);
        readFile(line, index, temp);
        float b = stringToFloat(temp);
        Patient p1(a, g, b, id, n, cont, pass);
        storPat.add(p1);
        delete[] n;
        delete[] cont;
        delete[] pass;
    }
    f.close();
}

void File::fileDoctor(Storage<Doctor>& storDoc) {
    ifstream f("doctors.txt");
    if (!f.is_open()) {
        throw FileNotFoundException("Error! doctors.txt could not be opened.");
    }
    char line[500];
    while (f.getline(line, 500)) {
        int index = 0;
        char temp[100];
        readFile(line, index, temp);
        int idValue = stringToFloat(temp);
        readFile(line, index, temp);
        char* n = new char[getLength(temp) + 1];
        copyStr(n, temp);
        readFile(line, index, temp);
        char* s = new char[getLength(temp) + 1];
        copyStr(s, temp);
        readFile(line, index, temp);
        char* c = new char[getLength(temp) + 1];
        copyStr(c, temp);
        readFile(line, index, temp);
        char* p = new char[getLength(temp) + 1];
        copyStr(p, temp);
        readFile(line, index, temp);
        float feeValue = stringToFloat(temp);
        Doctor d(s, feeValue, idValue, n, c, p);
        storDoc.add(d);
        delete[] s;
        delete[] n;
        delete[] c;
        delete[] p;
    }
    f.close();
}

void File::fileAppointments(Storage<Appointment>& storApp) {
    ifstream f("appointments.txt");
    if (!f.is_open()) {
        throw FileNotFoundException("Error! appointments.txt could not be opened.");
    }
    char line[500];
    while (f.getline(line, 500)) {
        int index = 0;
        char temp[100];
        readFile(line, index, temp);
        int appID = stringToFloat(temp);
        readFile(line, index, temp);
        int pID = stringToFloat(temp);
        readFile(line, index, temp);
        int dID = stringToFloat(temp);
        readFile(line, index, temp);
        char* date = new char[getLength(temp) + 1];
        copyStr(date, temp);
        readFile(line, index, temp);
        char* time = new char[getLength(temp) + 1];
        copyStr(time, temp);
        readFile(line, index, temp);
        char* status = new char[getLength(temp) + 1];
        copyStr(status, temp);
        Appointment a(appID, pID, dID, date, time, status);
        storApp.add(a);
        delete[] date;
        delete[] time;
        delete[] status;
    }
    f.close();
}

void File::fileAdmins(Storage<Admin>& storAdmin) {
    ifstream f("admin.txt");
    if (!f.is_open()) {
        throw FileNotFoundException("Error! admin.txt could not be opened.");
    }
    char line[500];
    while (f.getline(line, 500)) {
        int index = 0;
        char temp[100];
        readFile(line, index, temp);
        int id = stringToFloat(temp);
        readFile(line, index, temp);
        char* name = new char[getLength(temp) + 1];
        copyStr(name, temp);
        readFile(line, index, temp);
        char* pass = new char[getLength(temp) + 1];
        copyStr(pass, temp);
        Admin adm(id, name, pass);
        storAdmin.add(adm);
        delete[] name;
        delete[] pass;
    }
    f.close();
}

void File::fileBills(Storage<Bill>& storBill) {
    ifstream f("bills.txt");
    if (!f.is_open()) {
        throw FileNotFoundException("Error! bills.txt could not be opened.");
    }
    char line[500];
    while (f.getline(line, 500)) {
        int index = 0;
        char temp[100];
        readFile(line, index, temp);
        int bID = stringToFloat(temp);
        readFile(line, index, temp);
        int pID = stringToFloat(temp);
        readFile(line, index, temp);
        int appID = stringToFloat(temp);
        readFile(line, index, temp);
        float amt = stringToFloat(temp);
        readFile(line, index, temp);
        char* status = new char[getLength(temp) + 1];
        copyStr(status, temp);
        readFile(line, index, temp);
        char* date = new char[getLength(temp) + 1];
        copyStr(date, temp);
        Bill bill(bID, pID, appID, amt, status, date);
        storBill.add(bill);
        delete[] status;
        delete[] date;
    }
    f.close();
}

void File::filePrescriptions(Storage<Prescription>& storPres) {
    ifstream f("prescriptions.txt");
    if (!f.is_open()) {
        throw FileNotFoundException("Error! prescriptions.txt could not be opened.");
    }
    char line[1000];
    while (f.getline(line, 1000)) {
        int index = 0;
        char temp[500];
        readFile(line, index, temp);
        int prID = stringToFloat(temp);
        readFile(line, index, temp);
        int appID = stringToFloat(temp);
        readFile(line, index, temp);
        int patID = stringToFloat(temp);
        readFile(line, index, temp);
        int docID = stringToFloat(temp);
        readFile(line, index, temp);
        char* date = new char[getLength(temp) + 1];
        copyStr(date, temp);
        readFile(line, index, temp);
        char* meds = new char[getLength(temp) + 1];
        copyStr(meds, temp);
        readFile(line, index, temp);
        char* inst = new char[getLength(temp) + 1];
        copyStr(inst, temp);
        Prescription pres(prID, appID, patID, docID, date, meds, inst);
        storPres.add(pres);
        delete[] date;
        delete[] meds;
        delete[] inst;
    }
    f.close();
}

void File::savePatients(Storage<Patient>& storPat) {
    ofstream f("patients.txt");
    if (f.is_open()) {
        for (int i = 0; i < storPat.getSize(); i++) {
            Patient& p = storPat[i];
            f << p.getID() << "," << p.getName() << "," << p.getAge() << "," << p.getGender() << ","
                << p.getContact() << "," << p.getPass() << "," << p.getBalance();
            if (i < storPat.getSize() - 1) {
                f << endl;
            }
        }
        f.close();
    }
}

void File::saveDoctor(Storage<Doctor>& storDoc) {
    ofstream f("doctors.txt");
    if (f.is_open()) {
        for (int i = 0; i < storDoc.getSize(); i++) {
            Doctor& d = storDoc[i];
            f << d.getID() << "," << d.getName() << "," << d.getSpecialization() << "," << d.getContact()
                << "," << d.getPass() << "," << d.getFee();
            if (i < storDoc.getSize() - 1) {
                f << endl;
            }
        }
        f.close();
    }
}

void File::saveAppointment(Storage<Appointment>& storApp) {
    ofstream f("appointments.txt");
    if (f.is_open()) {
        for (int i = 0; i < storApp.getSize(); i++) {
            Appointment& a = storApp[i];
            f << a.getID() << "," << a.getPatID() << "," << a.getDocID() << "," << a.getDate() << ","
                << a.getTimeSlot() << "," << a.getStatus();
            if (i < storApp.getSize() - 1) {
                f << endl;
            }
        }
        f.close();
    }
}

void File::saveBills(Storage<Bill>& storBill) {
    ofstream f("bills.txt");
    if (f.is_open()) {
        for (int i = 0; i < storBill.getSize(); i++) {
            Bill& b = storBill[i];
            f << b.getID() << "," << b.getPatientID() << "," << b.getAppointmentID() << "," << b.getAmount()
                << "," << b.getStatus() << "," << b.getDate();
            if (i < storBill.getSize() - 1) {
                f << endl;
            }
        }
        f.close();
    }
}

void File::savePrescriptions(Storage<Prescription>& storPres) {
    ofstream f("prescriptions.txt");
    if (f.is_open()) {
        for (int i = 0; i < storPres.getSize(); i++) {
            Prescription& pres = storPres[i];
            f << pres.getID() << "," << pres.getAppointmentID() << "," << pres.getPatientID() << ","
                << pres.getDoctorID() << "," << pres.getDate() << "," << pres.getMedicines() << ","
                << pres.getNotes();
            if (i < storPres.getSize() - 1) {
                f << endl;
            }
        }
        f.close();
    }
}

void File::appendAppointment(Appointment& a) {
    ofstream f("appointments.txt", ios::app);
    if (f.is_open()) {
        f << endl << a.getID() << "," << a.getPatID() << "," << a.getDocID() << "," << a.getDate() << ","
            << a.getTimeSlot() << "," << a.getStatus();
        f.close();
    }
}

void File::appendBill(Bill& b) {
    ofstream f("bills.txt", ios::app);
    if (f.is_open()) {
        f << endl << b.getID() << "," << b.getPatientID() << "," << b.getAppointmentID() << "," << b.getAmount()
            << "," << b.getStatus() << "," << b.getDate();
        f.close();
    }
}

void File::appendPrescription(Prescription& p) {
    ofstream f("prescriptions.txt", ios::app);
    if (f.is_open()) {
        f << endl << p.getID() << "," << p.getAppointmentID() << "," << p.getPatientID() << ","
            << p.getDoctorID() << "," << p.getDate() << "," << p.getMedicines() << "," << p.getNotes();
        f.close();
    }
}

void File::appendDoctor(Doctor& d) {
    ofstream f("doctors.txt", ios::app);
    if (f.is_open()) {
        f << endl << d.getID() << "," << d.getName() << "," << d.getSpecialization() << "," << d.getContact()
            << "," << d.getPass() << "," << d.getFee();
        f.close();
    }
}

void File::writeSecurityLog(const char* role, int enteredID, const char* result) {
    ofstream f("security_log.txt", ios::app);
    if (f.is_open()) {
        time_t t = time(nullptr);
        tm* now = localtime(&t);   
        char timestamp[25];
        strftime(timestamp, 25, "%d-%m-%Y %H:%M:%S", now);
        f << timestamp << "," << role << "," << enteredID << "," << result << endl;
        f.close();
    }
}

void File::readSecurityLog() {
    ifstream f("security_log.txt");
    if (!f.is_open()) {
        cout << "No security events logged." << endl;
        return;
    }
    char line[300];
    bool hasContent = false;
    while (f.getline(line, 300)) {
        if (getLength(line) > 0) {
            cout << line << endl;
            hasContent = true;
        }
    }
    if (!hasContent) {
        cout << "No security events logged." << endl;
    }
    f.close();
}

void File::dischargePatient(int patientID, Storage<Patient>& storPat, Storage<Appointment>& storApp,
    Storage<Bill>& storBill, Storage<Prescription>& storPres) {
    ofstream f("discharged.txt", ios::app);
    if (f.is_open()) {
        Patient* p = storPat.findByID(patientID);
        if (p != nullptr) {
            f << p->getID() << "," << p->getName() << "," << p->getAge() << "," << p->getGender() << ","
                << p->getContact() << "," << p->getPass() << "," << p->getBalance() << endl;
        }
        for (int i = 0; i < storApp.getSize(); i++) {
            Appointment& a = storApp[i];
            if (a.getPatID() == patientID) {
                f << a.getID() << "," << a.getPatID() << "," << a.getDocID() << "," << a.getDate() << ","
                    << a.getTimeSlot() << "," << a.getStatus() << endl;
            }
        }
        for (int i = 0; i < storBill.getSize(); i++) {
            Bill& b = storBill[i];
            if (b.getPatientID() == patientID) {
                f << b.getID() << "," << b.getPatientID() << "," << b.getAppointmentID() << "," << b.getAmount()
                    << "," << b.getStatus() << "," << b.getDate() << endl;
            }
        }
        for (int i = 0; i < storPres.getSize(); i++) {
            Prescription& pr = storPres[i];
            if (pr.getPatientID() == patientID) {
                f << pr.getID() << "," << pr.getAppointmentID() << "," << pr.getPatientID() << ","
                    << pr.getDoctorID() << "," << pr.getDate() << "," << pr.getMedicines() << ","
                    << pr.getNotes() << endl;
            }
        }
        f.close();
    }
    storPat.removeByID(patientID);
    savePatients(storPat);
    for (int i = 0; i < storApp.getSize(); i++) {
        if (storApp[i].getPatID() == patientID) {
            storApp.removeByID(storApp[i].getID());
            i--;
        }
    }
    saveAppointment(storApp);
    for (int i = 0; i < storBill.getSize(); i++) {
        if (storBill[i].getPatientID() == patientID) {
            storBill.removeByID(storBill[i].getID());
            i--;
        }
    }
    saveBills(storBill);
    for (int i = 0; i < storPres.getSize(); i++) {
        if (storPres[i].getPatientID() == patientID) {
            storPres.removeByID(storPres[i].getID());
            i--;
        }
    }
    savePrescriptions(storPres);
}

void File::loadAllFiles(Storage<Patient>& p, Storage<Doctor>& d, Storage<Appointment>& a, Storage<Admin>& adm,
    Storage<Bill>& bill, Storage<Prescription>& pres) {
    filePatients(p);
    fileDoctor(d);
    fileAppointments(a);
    fileAdmins(adm);
    fileBills(bill);
    filePrescriptions(pres);
}

void File::saveAllFiles(Storage<Patient>& p, Storage<Doctor>& d, Storage<Appointment>& a, Storage<Bill>& bill,
    Storage<Prescription>& pres) {
    savePatients(p);
    saveDoctor(d);
    saveAppointment(a);
    saveBills(bill);
    savePrescriptions(pres);
}