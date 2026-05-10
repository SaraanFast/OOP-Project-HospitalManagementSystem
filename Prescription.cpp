#include "Prescription.hpp"
Prescription::Prescription() {
    prescriptionID = 0;
    appointmentID = 0;
    patientID = 0;
    doctorID = 0;
    date = nullptr;
    medicines = nullptr;
    notes = nullptr;
}

Prescription::Prescription(int pID, int appID, int patID, int docID, const char* d, const char* meds, const char* n) {
    prescriptionID = pID;
    appointmentID = appID;
    patientID = patID;
    doctorID = docID;
    date = new char[getLength(d) + 1];
    copyStr(date, d);
    medicines = new char[getLength(meds) + 1];
    copyStr(medicines, meds);
    notes = new char[getLength(n) + 1];
    copyStr(notes, n);
}

Prescription::~Prescription() {
    delete[] date;
    delete[] medicines;
    delete[] notes;
}

Prescription::Prescription(const Prescription& pres) {
    prescriptionID = pres.prescriptionID;
    appointmentID = pres.appointmentID;
    patientID = pres.patientID;
    doctorID = pres.doctorID;
    if (pres.date != nullptr) {
        date = new char[getLength(pres.date) + 1];
        copyStr(date, pres.date);
    }
    else {
        date = nullptr;
    }
    if (pres.medicines != nullptr) {
        medicines = new char[getLength(pres.medicines) + 1];
        copyStr(medicines, pres.medicines);
    }
    else {
        medicines = nullptr;
    }
    if (pres.notes != nullptr) {
        notes = new char[getLength(pres.notes) + 1];
        copyStr(notes, pres.notes);
    }
    else {
        notes = nullptr;
    }
}

Prescription& Prescription::operator=(const Prescription& pres) {
    if (this != &pres) {
        delete[] date;
        delete[] medicines;
        delete[] notes;
        prescriptionID = pres.prescriptionID;
        appointmentID = pres.appointmentID;
        patientID = pres.patientID;
        doctorID = pres.doctorID;
        if (pres.date != nullptr) {
            date = new char[getLength(pres.date) + 1];
            copyStr(date, pres.date);
        }
        else {
            date = nullptr;
        }
        if (pres.medicines != nullptr) {
            medicines = new char[getLength(pres.medicines) + 1];
            copyStr(medicines, pres.medicines);
        }
        else {
            medicines = nullptr;
        }
        if (pres.notes != nullptr) {
            notes = new char[getLength(pres.notes) + 1];
            copyStr(notes, pres.notes);
        }
        else {
            notes = nullptr;
        }
    }
    return *this;
}

int Prescription::getID() {
    return prescriptionID;
}

int Prescription::getAppointmentID() {
    return appointmentID;
}

int Prescription::getPatientID() {
    return patientID;
}

int Prescription::getDoctorID() {
    return doctorID;
}

const char* Prescription::getDate() {
    return date;
}

const char* Prescription::getMedicines() {
    return medicines;
}

const char* Prescription::getNotes() {
    return notes;
}

ostream& operator<<(ostream& out, const Prescription& p) {
    out << "Prescription ID: " << p.prescriptionID << " | Appointment ID: " << p.appointmentID<< " | Patient ID: "
        << p.patientID << " | Doctor ID: " << p.doctorID<< " | Date: " << (p.date ? p.date : "N/A")<<
        " | Medicines: " << (p.medicines ? p.medicines : "None")<< " | Doctor Notes: " << (p.notes ? p.notes : "None");
    return out;
}