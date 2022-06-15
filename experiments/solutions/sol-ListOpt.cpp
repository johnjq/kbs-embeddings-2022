#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
typedef std::pair<int, int> ii;
////////////////////////////////////////// BEGIN OF FRECHET //////////////////////////////////////////
// Note: 0-indexed vertices
// Given a tree with N vertices, computes an isometric embedding into R^d, under the L-inf metric, where d = O(log N)
namespace frechet_ {
    std::vector<std::vector<int>> adj;
    std::vector<std::vector<int>> vcoordinates;
    std::vector<bool> mark;
    std::vector<int> subtree_size;
    void compute_sizes(const int u, const int v) {
        subtree_size[v] = 1;
        for (const int w : adj[v]) if (w != u) {
            compute_sizes(v, w);
            subtree_size[v] += subtree_size[w];
        }
    }
    int find_centroid(const int r, const int u, const int v) {
        for (const int w : adj[v]) if (w != u)
            if (subtree_size[w] > subtree_size[r] / 2)
                return find_centroid(r, v, w);
        return v;
    }
    void find_all_reachable(const int u, const int v, std::vector<int>& reachable, std::vector<int>& distances, const int dist = 0) {
        reachable.push_back(v);
        distances.push_back(dist);
        for (const int w : adj[v]) if (w != u)
            find_all_reachable(v, w, reachable, distances, dist + 1);
    }
    vector<int> disable(const int centroid, const std::vector<int>& vec) {
        vector<int> ret;
        for (const int x : vec)
            mark[x] = true;
        for (const int x : adj[centroid])
            if (!mark[x])
                ret.push_back(x);
        for (const int x : vec)
            mark[x] = false;
        return ret;
    }
    void compute_embedding(const int s) {
        // Compute subtree sizes
        compute_sizes(-1, s);
        // Handle base cases
        if (subtree_size[s] == 1) {
            return;
        } else if (subtree_size[s] == 2) {
            const int t = *adj[s].begin();
            vcoordinates[s].push_back(0);
            vcoordinates[t].push_back(1);
            return;
        }
        // Find centroid
        const int centroid = find_centroid(s, -1, s);
        // Establish splitting
        compute_sizes(-1, centroid);
        std::vector<int> positive, negative;
        int positive_size = 0, limit = 2*subtree_size[centroid]/3;
        for (const int v : adj[centroid]) {
            if (positive_size + subtree_size[v] <= limit) {
                positive_size += subtree_size[v];
                positive.push_back(v);
            } else {
                negative.push_back(v);
            }
        }
        if (negative.empty()) {
            negative.push_back(positive.back());
            positive.pop_back();
        }
        int size_pos = 0, size_neg = 0;
        for (int v : positive) size_pos += subtree_size[v];
        for (int v : negative) size_neg += subtree_size[v];
        if (size_neg <= size_pos) {
            vector<int> new_positive;
            for (int v : positive) {
                const int new_size_pos = size_pos - subtree_size[v];
                const int new_size_neg = size_neg + subtree_size[v];
                if (new_size_neg <= new_size_pos) {
                    negative.push_back(v);
                    size_pos = new_size_pos;
                    size_neg = new_size_neg;
                } else {
                    new_positive.push_back(v);
                }
            }
            swap(positive, new_positive);
        } else {
            assert(size_pos <= size_neg);
            vector<int> new_negative;
            for (int v : negative) {
                const int new_size_pos = size_pos + subtree_size[v];
                const int new_size_neg = size_neg - subtree_size[v];
                if (new_size_pos <= new_size_neg) {
                    positive.push_back(v);
                    size_pos = new_size_pos;
                    size_neg = new_size_neg;
                } else {
                    new_negative.push_back(v);
                }
            }
            swap(negative, new_negative);
        }
        // Compute the new centroid adjacency list for each of the subtrees
        vector<int> adj_pos = disable(centroid, positive);
        vector<int> adj_neg = disable(centroid, negative);
        // Compute embedding for the positive subtree
        swap(adj[centroid], adj_pos);
        compute_embedding(centroid);
        const int dimensionality_pos = vcoordinates[centroid].size();
        std::vector<int> reachable_pos, distances_pos;
        find_all_reachable(-1, centroid, reachable_pos, distances_pos);
        for (const int v : reachable_pos) if (v != centroid)
            for (int i = 0, sz = vcoordinates[v].size(); i < sz; ++i)
                vcoordinates[v][i] -= vcoordinates[centroid][i];
        vcoordinates[centroid].clear();
        swap(adj[centroid], adj_pos);
        // Compute embedding for the negative subtree
        swap(adj[centroid], adj_neg);
        compute_embedding(centroid);
        const int dimensionality_neg = vcoordinates[centroid].size();
        std::vector<int> reachable_neg, distances_neg;
        find_all_reachable(-1, centroid, reachable_neg, distances_neg);
        for (int v : reachable_neg) if (v != centroid)
            for (int i = 0, sz = vcoordinates[v].size(); i < sz; ++i)
                vcoordinates[v][i] -= vcoordinates[centroid][i];
        for (int& x : vcoordinates[centroid])
            x = 0;
        swap(adj[centroid], adj_neg);
        // Pad the lower-dimensional embedding
        if (dimensionality_pos > dimensionality_neg) {
            const int diff = dimensionality_pos - dimensionality_neg;
            for (int v : reachable_neg)
                for (int i = 0; i < diff; ++i)
                    vcoordinates[v].push_back(0);
        } else if (dimensionality_neg > dimensionality_pos) {
            const int diff = dimensionality_neg - dimensionality_pos;
            for (int v : reachable_pos) if (v != centroid)
                for (int i = 0; i < diff; ++i)
                    vcoordinates[v].push_back(0);
        }
        for (int i = 0, sz = reachable_pos.size(); i < sz; ++i)
            vcoordinates[reachable_pos[i]].push_back(distances_pos[i]);
        for (int i = 1, sz = reachable_neg.size(); i < sz; ++i)
            vcoordinates[reachable_neg[i]].push_back(-distances_neg[i]);
    }
}
std::vector<std::vector<int>> compute_frechet_embedding(const std::vector<std::pair<int, int>>& edges) {
    int n = 0;
    for (const std::pair<int, int>& edge : edges)
        n = std::max(n, std::max(edge.first, edge.second));
    n += 1;
    assert(n == (int)edges.size()+1);
    frechet_::adj.assign(n, {});
    frechet_::vcoordinates.assign(n, {});
    frechet_::mark.assign(n, false);
    frechet_::subtree_size.assign(n, 0);
    for (const std::pair<int, int>& edge : edges) {
        frechet_::adj[edge.first].push_back(edge.second);
        frechet_::adj[edge.second].push_back(edge.first);
    }
    frechet_::compute_embedding(0);
    return frechet_::vcoordinates;
}
////////////////////////////////////////// END OF FRECHET //////////////////////////////////////////

int32_t main() {
    // Read the input tree
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n;
    cin>>n;
    vector<pair<int, int>> edges;
    for (int i = 1; i < n; ++i) {
        int u, v;
        cin>>u>>v;
        edges.emplace_back(u, v);
    }
    // Compute an isometric embedding
    auto embedding = compute_frechet_embedding(edges);
    // Print dimensionality
    cout<<embedding[0].size()<<endl;
}
