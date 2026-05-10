#ifndef PERSON_HPP
#define PERSON_HPP
#include "helperFunctions.hpp"
class Person {
protected:
    int ID;
    char* name;
    char* contact;
    char* password;

public:
    Person();
    Person(int id, char* n, char* cont, char* pass);
    Person(const Person& p);
    Person& operator=(const Person& p);
    virtual ~Person();
    virtual void display() = 0; 
    virtual void menu() = 0; 
    int getID();
    char* getName();
    char* getContact();
    char* getPass();
};

#endif