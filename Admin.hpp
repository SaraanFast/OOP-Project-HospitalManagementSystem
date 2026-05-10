#ifndef ADMIN_HPP
#define ADMIN_HPP
#include "Person.hpp"
class Admin : public Person {
public:
    Admin();
    Admin(int id, char* n, char* pass);
    ~Admin();
    void display();
    void menu();
};
#endif