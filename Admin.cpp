#include "Admin.hpp"
#include <iostream>
using namespace std;
Admin::Admin() : Person() {}
Admin::Admin(int id, char* n, char* pass) : Person(id, n,nullptr, pass) {}
Admin::~Admin() {}
void Admin::display() {
    cout << "Admin ID: " << ID << " | Name: " << name << endl;
}
void Admin::menu() {
    // Logic in HospitalSystem.cpp
}