#ifndef APPOINTMENT_HPP 
#define APPOINTMENT_HPP
#include "helperFunctions.hpp"
#include<iostream>
using namespace std;
class Appointment {
private:
	int appointmentID;
	int patientID;
	int doctorID;
	char* date;
	char* timeSlot;
	char* status;
public:
	Appointment();
	Appointment(int appID, int pID, int docID, const char* d, const char* ts, const char* st);
	Appointment(const Appointment& ap);
	Appointment& operator=(const Appointment& ap);
	~Appointment();
	bool operator==(const Appointment& ap);
	friend ostream& operator<<(ostream& out, const Appointment& ap);
	int getID();
	int getPatID();
	const char* getStatus();
	void setStatus(const char* s);
	int getDocID();
	char* getDate();
	char* getTimeSlot();
};

#endif