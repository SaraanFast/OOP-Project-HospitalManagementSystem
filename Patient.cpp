#include "Patient.hpp"
#include "HospitalException.hpp"
Patient::Patient() {
    age = 0;
    gender = 'X';
    balance = 0;
}
Patient::Patient(int a, char g, float bal, int id, char* n, char* cont, char* pass)
    : Person(id, n, cont, pass) {
    age = a;
    gender = g;
    balance = bal;
}
Patient::Patient(const Patient& pat) : Person(pat) {
    age = pat.age;
    gender = pat.gender;
    balance = pat.balance;
}

Patient& Patient::operator=(const Patient& pat) {
    if (this != &pat) {
        Person::operator=(pat);
        age = pat.age;
        gender = pat.gender;
        balance = pat.balance;
    }
    return *this;
}
Patient::~Patient() {}
Patient& Patient::operator+=(float b) {
    balance = balance + b;
    return *this;
}
Patient& Patient::operator-=(float b) {
    if (balance >= b) {
        balance = balance - b;
    }
    else throw InsufficientFundsException();
    return *this;
}
bool Patient::operator==(const Patient& p) {
    return this->ID == p.ID;
}
void Patient::display() {
    cout << *this;
}
void Patient::menu() {
    // Logic in HospitalSystem.cpp
}
ostream& operator<<(ostream& out, const Patient& p) {
    out << "Patient: " << p.name << " | ID: " << p.ID << " | Balance: " << p.balance;
    return out;
}
int Patient::getAge() {
    return age;
}
char Patient::getGender() {
    return gender;
}
float Patient::getBalance() {
    return balance;
}