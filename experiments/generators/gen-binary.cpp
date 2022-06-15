#include <bits/stdc++.h>
#include "jngen.h"
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
typedef std::pair<int, int> ii;

int32_t main(int argc, char** argv) {
    registerGen(argc, argv);

    int n;
    cin >> n;
    cout << n << endl;
    cout << Tree::binary(n).shuffled() << endl;
}
