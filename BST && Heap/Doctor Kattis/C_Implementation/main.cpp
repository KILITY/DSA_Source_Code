#include <iostream>
#include <string>
#include "rbtree.h"
#include "hash_set.h"

using namespace std;

int compare_patients(const Patient* a, const Patient* b);

class Patient {
public:
    int severity;
    int arrival;
    string name;
    Patient() = default;
    Patient(string n, int s, int a) : severity(s), arrival(a), name(move(n)) {}
};

int compare_patients(const Patient* a, const Patient* b) {
    if (a->severity != b->severity)
        return b->severity - a->severity;  // higher severity first
    return a->arrival - b->arrival;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    RBTree* patients = rbtree_create(compare_patients);
    hash_map_t* dict = init_hash_map();
    int t, arrival = 0;
    cin >> t;

    while (t--) {
        int cmd;
        cin >> cmd;
        string name;
        if (cmd == 0) {
            int sev;
            cin >> name >> sev;
            Patient* p = new Patient(name, sev, arrival++);
            rbtree_insert(patients, p);
            hash_map_put(dict, name.c_str(), p);
        }
        else if (cmd == 1) {
            int inc;
            cin >> name >> inc;
            Patient* p = (Patient*)hash_map_get(dict, name.c_str());
            if (p) {
                rbtree_delete(patients, p);
                p->severity += inc;
                rbtree_insert(patients, p);
            }
        }
        else if (cmd == 2) {
            cin >> name;
            Patient* p = (Patient*)hash_map_get(dict, name.c_str());
            if (p) {
                rbtree_delete(patients, p);
                hash_map_delete(dict, name.c_str());
                delete p;
            }
        }
        else if (cmd == 3) {
            if (rbtree_empty(patients)) {
                cout << "The clinic is empty\n";
            } else {
                Patient* p = rbtree_min(patients);
                cout << p->name << "\n";
            }
        }
    }

    rbtree_free(patients);
    free_hash_map(dict);
    return 0;
}

/*
* credits to this REPO:
* https://github.com/TheAlgorithms/C/tree/master/data_structures
* And this guy
*  https://www.youtube.com/@MichaelSambol
*/