#include <bits/stdc++.h>
#include "jngen.h"
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
typedef std::pair<int, int> ii;

Tree generate_banana(const int n, const int k) {
    assert(n >= 1);
    assert(k >= 1);
    Tree master = Tree::star(1);
    int remaining_vertices = n - 1;
    while (remaining_vertices > 0) {
        const int used = min(remaining_vertices, k);
        const Tree star = Tree::star(used);
        const int leaf = used == 1 ? 0 : 1;
        master = master.link(0, star, leaf);
        remaining_vertices -= used;
    }
    return master;
}

int32_t main(int argc, char** argv) {
    registerGen(argc, argv);

    int n;
    cin >> n;
    const int k = sqrt(n);
    cout << n << endl;
    cout << generate_banana(n, k).shuffled() << endl;
}
