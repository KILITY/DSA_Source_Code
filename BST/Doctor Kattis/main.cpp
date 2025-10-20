// Problem link : https://open.kattis.com/problems/doctorkattis

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <stack>
#include <map>
#include <unordered_map>
#include <tuple>
#include <set>
#include <numeric>
#include <limits> //i don't have bits/stdc++.h on MacOs

using namespace std;

class Patient {
    public:
        int severity;
        int arrival;
        string name;

    Patient() = default; //for unordered_map

    Patient(string name, int severity, int arrival)
    : name(move(name)), severity(severity), arrival(arrival) {}


    bool operator<(const Patient& other) const {
        if (severity != other.severity) {
            return severity > other.severity; // higher severity first
        }
        return arrival < other.arrival; // earlier arrival first
    }
};

int main() {
    ios::sync_with_stdio( false );
    cin.tie( nullptr ); //these two lines are for SPEED :)

    set<Patient> patients;
    unordered_map<string, Patient> patient_dictionary;
    int t, arrival = 0;

    cin >> t;

    while ( t-- ) {
        int command;
        cin >> command;
        string patient_name;

        if ( command == 0 ) {
            int severity;
            cin >> patient_name >> severity;
            Patient p(patient_name, severity, arrival++);
            patients.insert(p);
            patient_dictionary[patient_name] = p;
        }
        else if ( command == 1 ) {
            int increase;
            cin >> patient_name >> increase;
            auto it = patient_dictionary.find(patient_name);
            if ( it != patient_dictionary.end() ) {
                patients.erase(it->second);
                it->second.severity += increase;
                patients.insert(it->second);
            }
        }
        else if ( command == 2 ) {
                cin >> patient_name;
                auto it = patient_dictionary.find(patient_name);
                if ( it != patient_dictionary.end() ) {
                    patients.erase(it->second);
                    patient_dictionary.erase(it);
            }
        }
        else if ( command == 3 ) {
            if ( patients.empty() ) {
                cout << "The clinic is empty\n";
            }
            else {
                const Patient& next_patient = *patients.begin();
                cout << next_patient.name << "\n";
            }
        }



    }
    return 0;
}