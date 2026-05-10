#include "Doctor.hpp"
#include <iostream>
using namespace std;
Doctor::Doctor() {
    specialization = nullptr;
    fee = 0;
}
Doctor::Doctor(char* spec, float f, int id, char* n, char* cont, char* pass): Person(id, n, cont, pass) {
    specialization = new char[getLength(spec) + 1];
    copyStr(specialization, spec);
    fee = f;
}
Doctor::Doctor(const Doctor& doc) : Person(doc) {
    fee = doc.fee;
    if (doc.specialization != nullptr) {
        specialization = new char[getLength(doc.specialization) + 1];
        copyStr(specialization, doc.specialization);
    }
    else {
        specialization = nullptr;
    }
}
Doctor& Doctor::operator=(const Doctor& doc) {
    if (this != &doc) {
        Person::operator=(doc);
        delete[] specialization;
        fee = doc.fee;
        if (doc.specialization != nullptr) {
            specialization = new char[getLength(doc.specialization) + 1];
            copyStr(specialization, doc.specialization);
        }
        else {
            specialization = nullptr;
        }
    }
    return *this;
}
Doctor::~Doctor() {
    delete[] specialization;
}
bool Doctor::operator==(const Doctor& d) {
    if (this->ID == d.ID) {
        return true;
    }
    return false;
}
void Doctor::display() {
    cout << *this;
}
void Doctor::menu() {
    // Logic in HospitalSystem.cpp
}
ostream& operator<<(ostream& out, const Doctor& d) {
    out << "Doctor: " << d.name << " | Specialization: " << d.specialization << " | Fee: " << d.fee;
    return out;
}
char* Doctor::getSpecialization(){
    return specialization; 
}
float Doctor::getFee() {
    return fee;
}