//Problem link: https://codeforces.com/problemset/problem/20/C

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

using ll = long long;
const ll INF = LLONG_MAX / 4; //A large value representing infinity divide by 4 to avoid overflow when adding weights

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr); //SPEED!!!

    int nodeCount, edgeCount;
    cin >> nodeCount >> edgeCount;

    vector<vector<pair<int, ll>>> graph(nodeCount + 1);

    for (int i = 0; i < edgeCount; i++) {
        int from, to;
        ll weight;
        cin >> from >> to >> weight;

        //for this problem we have an undirected graph, for directed graphs just add one direction instead of both and check only outgoing edges
        graph[from].push_back({to, weight});
        graph[to].push_back({from, weight});
    }

    vector<ll> minDistance(nodeCount + 1, INF);
    vector<int> previousNode(nodeCount + 1, -1);

    // From point A to point A of course that the distance is 0
    minDistance[1] = 0;

    // We always go to the next node with the smallest distance
    priority_queue<
        pair<ll,int>,
        vector<pair<ll,int>>,
        greater<pair<ll,int>>
    > pq;

    pq.push({0, 1});

    while (!pq.empty()) {
        auto [currentDist, currentNode] = pq.top();
        pq.pop();

        // We can have outdated entries in the priority queue so we ignore them, we store in the minDistance array the best known distance to each node
        if (currentDist > minDistance[currentNode]) continue;

        for (auto &edge : graph[currentNode]) { // Explore all neighbors
            int neighbor = edge.first;
            ll edgeWeight = edge.second;

            ll newDistance = minDistance[currentNode] + edgeWeight; // The distance to one of the neighbors might be shorter if we go through the shortest path that passes through the current node

            if (newDistance < minDistance[neighbor]) {
                minDistance[neighbor] = newDistance;
                previousNode[neighbor] = currentNode; //Using the previousNode array to rebuild the path later
                pq.push({newDistance, neighbor});
            }
        }
    }

    // Please invent teleportation technology if there is no path
    if (minDistance[nodeCount] == INF) {
        cout << "-1\n";
        return 0;
    }

    // Using the previousNode array we rebuild the path from the last node to the first one
    vector<int> path;
    for (int node = nodeCount; node != -1; node = previousNode[node]) {
        path.push_back(node);
    }
    reverse(path.begin(), path.end());

    for (int node : path) {
        cout << node << " ";
    }
    cout << "\n";

    return 0;
}