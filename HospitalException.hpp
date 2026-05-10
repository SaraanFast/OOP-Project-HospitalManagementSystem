#ifndef HOSPITAL_EXCEPTION_HPP
#define HOSPITAL_EXCEPTION_HPP
#include "helperFunctions.hpp"
class HospitalException {
protected:
    char errMes[200];
public:
    HospitalException(const char* msg) {
        copyStr(errMes, msg);
    }
    virtual ~HospitalException() {}
    virtual const char* what() const {
        return errMes;
    }
};
    class FileNotFoundException : public HospitalException {
    public:
        FileNotFoundException(const char* msg = "Error! File could not be opened"): HospitalException(msg) { }
    };
    class InsufficientFundsException : public HospitalException {
    public:
        InsufficientFundsException(const char* msg = "Error!Insufficient funds in patient's account."): HospitalException(msg) {}
    };

    class InvalidInputException : public HospitalException {
    public:
        InvalidInputException(const char* msg = "Error!User details failed validation tests"): HospitalException(msg) {}
    };

    class SlotUnavailableException : public HospitalException {
    public:
        SlotUnavailableException(const char* msg = "Error!The selected appointment slot is already booked by another patient")
            : HospitalException(msg) {}
};
#endif