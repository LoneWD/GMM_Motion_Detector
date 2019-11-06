#include "gm.hpp"
#include <bits/stdc++.h>
using namespace std;


int main() {
    vector<int> g = {1, 5, 3, 7, 2};
    g.erase(g.begin() + 3);
    for (auto x:g) {
        cout << x << endl;
    }
}