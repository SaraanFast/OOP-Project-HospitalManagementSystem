#ifndef DOCTOR_HPP
#define DOCTOR_HPP
#include "Person.hpp"
using namespace std;
class Doctor : public Person {
private:
    char* specialization;
    float fee;

public:
    Doctor();
    Doctor(char* spec, float f, int id, char* n, char* cont, char* pass);
    Doctor(const Doctor& other);
    Doctor& operator=(const Doctor& other);
    ~Doctor();
    bool operator==(const Doctor& d);
    void display()override;
    void menu()override;
    friend ostream& operator<<(ostream& out, const Doctor& d);
    char* getSpecialization();
    float getFee();
};

#endif