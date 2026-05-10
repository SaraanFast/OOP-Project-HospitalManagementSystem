#ifndef STORAGE_HPP
#define STORAGE_HPP
#include <iostream>
using namespace std;
template <typename T>
class Storage {
private:
    T* data;
    int size;
public:
    Storage() {
        size = 0;
        data = new T[100];
    }
    Storage(const Storage& other) {
        size = other.size;
        data = new T[100];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }
    Storage& operator=(const Storage& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new T[100];
            for (int i = 0; i < size; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    ~Storage() {
        delete[] data;
    }
    void add(const T& item) {
        if (size < 100) {
            data[size] = item;
            size++;
        }
        else {
            cout << "Storage is full,cant add more items" << endl;
        }
    }
    int getSize() {
        return size;
    }
    T& operator[](int index) {
        return data[index];
    }
    void display() {
        for (int i = 0; i < size; i++) {
            cout << data[i] << endl;
        }
    }
    T* findByID(int id) {
        for (int i = 0; i < size; i++) {
            if (data[i].getID() == id) {
                return &data[i];
            }
        }
        return nullptr;
    }
    bool removeByID(int id) {
        int index = -1;
        for (int i = 0; i < size; i++) {
            if (data[i].getID() == id) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            for (int i = index; i < size - 1; i++) {
                data[i] = data[i + 1];
            }
            size--;
            return true;
        }
        return false;
    }
    T* getData() {
        return data;
    }
};

#endif