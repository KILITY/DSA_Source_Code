//Problem link: https://codeforces.com/problemset/problem/1095/C
// Solving ideea since we need powers of two we take the bit representation of n which literraly means sum of powers of two that equal n,
// and if we dont have enough we keep splitting the largest power of two until we reach k parts or we cant split anymore

#include <iostream>
#include <string>
#include <queue>
//again i don't have bits/stdc++.h on MacOs

using namespace std;

int main() {
    ios::sync_with_stdio(false); //SPEED!!!
    cin.tie(nullptr);

    long long n;
    int k;
    cin >> n >> k;

    //Store bits
    priority_queue<int> pq;
    for (int i = 0; i < 63; ++i) {
        if (n & (1LL << i))
            pq.push(1 << i);
    }

    //Too many bits
    if ((int)pq.size() > k) {
        cout << "NO\n";
        return 0;
    }

    //Too few bits , split the top one if not 1, until we reach k parts
    while ((int)pq.size() < k) {
        int largest = pq.top();
        pq.pop();

        if (largest == 1) {
            cout << "NO\n";
            return 0;
        }

        pq.push(largest / 2);
        pq.push(largest / 2);
    }

    cout << "YES\n";
    while (!pq.empty()) {
        cout << pq.top() << " ";
        pq.pop();
    }
    cout << "\n";

    return 0;
}