#include "HospitalSystem.hpp"
#include <iostream>
using namespace std;
int main() {
    try {
        cout << "Starting  Hospital Management System..." << endl;
        HospitalManagementSystem hms;
        hms.run();
    }
    catch (const HospitalException& e) {
        cout << " SYSTEM ERROR "<< endl;
        cout << e.what() << endl;
    }
    catch (const std::exception& e) {
        cout << " UNKNOWN EXCEPTION" << endl;
        cout << e.what() << endl;
    }
    catch (...) {
        cout << "CRITICAL UNKNOWN ERROR" << endl;
    }
    cout << "Program terminated" << endl;
    return 0;
}