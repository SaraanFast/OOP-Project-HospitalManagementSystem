#include <iostream>
using namespace std;
#include "Appointment.hpp"
#include "helperFunctions.hpp"
Appointment::Appointment() {
	appointmentID = 0;
	doctorID = 0;
	patientID = 0;
	date = nullptr;
	timeSlot = nullptr;
	status = nullptr;
}

Appointment::Appointment(int appID, int pID, int docID, const char* d, const char* ts, const char* st) {
	appointmentID = appID;
	doctorID = docID;
	patientID = pID;
	date = new char[getLength(d) + 1];
	copyStr(date, d);
	timeSlot = new char[getLength(ts) + 1];
	copyStr(timeSlot, ts);
	status = new char[getLength(st) + 1];
	copyStr(status, st);
}

Appointment::Appointment(const Appointment& ap) {
	appointmentID = ap.appointmentID;
	doctorID = ap.doctorID;
	patientID = ap.patientID;
	if (ap.date != nullptr) {
		date = new char[getLength(ap.date) + 1];
		copyStr(date, ap.date);
	}
	else {
		date = nullptr;
	}
	if (ap.timeSlot != nullptr) {
		timeSlot = new char[getLength(ap.timeSlot) + 1];
		copyStr(timeSlot, ap.timeSlot);
	}
	else {
		timeSlot = nullptr;
	}
	if (ap.status != nullptr) {
		status = new char[getLength(ap.status) + 1];
		copyStr(status, ap.status);
	}
	else {
		status = nullptr;
	}
}

Appointment& Appointment::operator=(const Appointment& ap) {
	if (this != &ap) {
		delete[] date;
		delete[] timeSlot;
		delete[] status;
		appointmentID = ap.appointmentID;
		doctorID = ap.doctorID;
		patientID = ap.patientID;
		if (ap.date != nullptr) {
			date = new char[getLength(ap.date) + 1];
			copyStr(date, ap.date);
		}
		else {
			date = nullptr;
		}
		if (ap.timeSlot != nullptr) {
			timeSlot = new char[getLength(ap.timeSlot) + 1];
			copyStr(timeSlot, ap.timeSlot);
		}
		else {
			timeSlot = nullptr;
		}
		if (ap.status != nullptr) {
			status = new char[getLength(ap.status) + 1];
			copyStr(status, ap.status);
		}
		else {
			status = nullptr;
		}
	}
	return *this;
}

Appointment::~Appointment() {
	delete[] date;
	delete[] timeSlot;
	delete[] status;
}

bool Appointment::operator==(const Appointment& ap) {
	bool isSameDoc = (doctorID == ap.doctorID);
	bool isSameDay = isEqual(date, ap.date);
	bool isSameSlot = isEqual(timeSlot, ap.timeSlot);
	if (isSameDoc && isSameDay && isSameSlot) {
		if (!isEqual(status, "cancelled") && !isEqual(ap.status, "cancelled")) {
			return true;
		}
	}
	return false;
}

ostream& operator<<(ostream& out, const Appointment& ap) {
	out << "Appointment ID: " << ap.appointmentID << "|Patient ID: " << ap.patientID << "|Doctor ID: " << ap.doctorID
		<< "|Date: " << ap.date << "|Time Slot: " << ap.timeSlot << "|Status: " << ap.status << endl;
	return out;
}

int Appointment::getID() {
	return appointmentID;
}

int Appointment::getPatID() {
	return patientID;
}

const char* Appointment::getStatus() {
	return status;
}

void Appointment::setStatus(const char* s) {
	delete[] status;
	status = new char[getLength(s) + 1];
	copyStr(status, s);
}

int Appointment::getDocID() {
	return doctorID;
}

char* Appointment::getDate() {
	return date;
}

char* Appointment::getTimeSlot() {
	return timeSlot;
}