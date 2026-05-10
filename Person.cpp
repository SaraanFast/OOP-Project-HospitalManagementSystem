#include "Person.hpp"
Person::Person() {
    ID = 0;
    name = nullptr;
    contact = nullptr;
    password = nullptr;
}
Person::Person(int id, char* n, char* cont, char* pass) {
    ID = id;
    name = new char[getLength(n) + 1];
    copyStr(name, n);
    if (cont != nullptr) {
        contact = new char[getLength(cont) + 1];
        copyStr(contact, cont);
    }
    else {
        contact = new char[4];
        copyStr(contact,"N/A");
    }
    password = new char[getLength(pass) + 1];
    copyStr(password, pass);
}
Person::Person(const Person& p) {
    ID = p.ID;
    if (p.name != nullptr) {
        name = new char[getLength(p.name) + 1];
        copyStr(name, p.name);
    }
    else {
        name = nullptr;
    }
    if (p.contact != nullptr) {
        contact = new char[getLength(p.contact) + 1];
        copyStr(contact, p.contact);
    }
    else {
        contact = nullptr;
    }
    if (p.password != nullptr) {
        password = new char[getLength(p.password) + 1];
        copyStr(password, p.password);
    }
    else {
        password = nullptr;
    }
}

Person& Person::operator=(const Person& other) {
    if (this != &other) {
        delete[] name;
        delete[] contact;
        delete[] password;
        ID = other.ID;
        if (other.name != nullptr) {
            name = new char[getLength(other.name) + 1];
            copyStr(name, other.name);
        }
        else {
            name = nullptr;
        }
        if (other.contact != nullptr) {
            contact = new char[getLength(other.contact) + 1];
            copyStr(contact, other.contact);
        }
        else {
            contact = nullptr;
        }
        if (other.password != nullptr) {
            password = new char[getLength(other.password) + 1];
            copyStr(password, other.password);
        }
        else {
            password = nullptr;
        }
    }
    return *this;
}
Person::~Person() {
    delete[] name;
    delete[] contact;
    delete[] password;
}
int Person::getID() {
    return ID;
}
char* Person::getName() {
    return name;
}
char* Person::getContact() {
    return contact;
}
char* Person::getPass() {
    return password;
}