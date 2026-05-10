#ifndef PATIENT_HPP
#define PATIENT_HPP
#include "Person.hpp"
#include <iostream>
using namespace std;
class Patient : public Person {
private:
    int age;
    char gender;
    float balance;

public:
    Patient();
    Patient(int a, char g, float bal, int id, char* n, char* cont, char* pass);
    Patient(const Patient& pat);
    Patient& operator=(const Patient& pat);
    ~Patient();
    Patient& operator+=(float b);
    Patient& operator-=(float b);
    bool operator==(const Patient& p);
    void display ()override;
    void menu()override;
    friend ostream& operator<<(ostream& out, const Patient& p);
    int getAge();
    char getGender();
    float getBalance();
};
#endif