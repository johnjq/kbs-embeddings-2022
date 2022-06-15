#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
typedef std::pair<int, int> ii;
////////////////////////////////////////// BEGIN OF FRECHET //////////////////////////////////////////
// Note: 0-indexed vertices
// Given a tree with N vertices, computes an isometric embedding into R^d, under the L-inf metric, where d = O(log N)
namespace frechet_ {
    std::vector<std::vector<int>> global_adj;
    std::vector<int> subtree_size;
    void compute_sizes(std::vector<std::vector<int>>& adj, const int u, const int v) {
        subtree_size[v] = 1;
        for (const int w : adj[v]) if (w != u) {
            compute_sizes(adj, v, w);
            subtree_size[v] += subtree_size[w];
        }
    }
    int find_centroid(std::vector<std::vector<int>>& adj, const int r, const int u, const int v) {
        for (const int w : adj[v]) if (w != u)
            if (subtree_size[w] > subtree_size[r] / 2)
                return find_centroid(adj, r, v, w);
        return v;
    }
    void find_all_reachable(std::vector<std::vector<int>>& adj, const int u, const int v, std::vector<int>& reachable, std::vector<int>& distances, const int dist) {
        reachable.push_back(v);
        distances.push_back(dist);
        for (const int w : adj[v]) if (w != u)
            find_all_reachable(adj, v, w, reachable, distances, dist + 1);
    }
    std::vector<std::vector<int>> compute_embedding(std::vector<std::vector<int>>& adj) {
        // Handle base cases
        const int n = adj.size();
        if (n == 1) {
            return {{}};
        } else if (n == 2) {
            return {{0}, {1}};
        }
        // Compute subtree sizes
        compute_sizes(adj, -1, 0);
        // Find centroid
        const int centroid = find_centroid(adj, 0, -1, 0);
        // Establish partitions
        compute_sizes(adj, -1, centroid);
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
        // Discover reachable vertices
        std::vector<int> reachable_pos, reachable_neg;
        std::vector<int> distances_pos, distances_neg;
        reachable_pos.push_back(centroid);
        reachable_neg.push_back(centroid);
        distances_pos.push_back(0);
        distances_neg.push_back(0);
        for (int v : positive)
            find_all_reachable(adj, centroid, v, reachable_pos, distances_pos, 1);
        for (int v : negative)
            find_all_reachable(adj, centroid, v, reachable_neg, distances_neg, 1);
        // Reindex vertices
        std::vector<int> new_index(n);
        int idx_pos = 0, idx_neg = 0;
        for (int v : reachable_pos)
            new_index[v] = idx_pos++;
        for (int v : reachable_neg)
            new_index[v] = idx_neg--;
        // Build subtrees
        std::vector<std::vector<int>> adj_pos(reachable_pos.size()), adj_neg(reachable_neg.size());
        for (int v : reachable_pos)
            for (int w : adj[v])
                if (new_index[w] >= 0)
                    adj_pos[new_index[v]].push_back(new_index[w]);
        for (int v : reachable_neg)
            for (int w : adj[v])
                if (new_index[w] <= 0)
                    adj_neg[-new_index[v]].push_back(-new_index[w]);
        // Compute subtree embeddings
        std::vector<std::vector<int>> embedding_pos = compute_embedding(adj_pos);
        std::vector<std::vector<int>> embedding_neg = compute_embedding(adj_neg);
        // Translate centroid to the origin
        const int embedding_pos_dim = embedding_pos[0].size();
        const int embedding_neg_dim = embedding_neg[0].size();
        for (int i = 1, sz = embedding_pos.size(); i < sz; ++i)
            for (int j = 0; j < embedding_pos_dim; ++j)
                embedding_pos[i][j] -= embedding_pos[0][j];
        for (int i = 1, sz = embedding_neg.size(); i < sz; ++i)
            for (int j = 0; j < embedding_neg_dim; ++j)
                embedding_neg[i][j] -= embedding_neg[0][j];
        for (int i = 0; i < embedding_pos_dim; ++i)
            embedding_pos[0][i] = 0;
        for (int i = 0; i < embedding_neg_dim; ++i)
            embedding_neg[0][i] = 0;
        // Pad lower-dimensional embedding
        if (embedding_pos_dim < embedding_neg_dim) {
            for (std::vector<int>& coord : embedding_pos)
                while ((int) coord.size() < embedding_neg_dim)
                    coord.push_back(0);
        } else if (embedding_neg_dim < embedding_pos_dim) {
            for (std::vector<int>& coord : embedding_neg)
                while ((int) coord.size() < embedding_pos_dim)
                    coord.push_back(0);
        }
        // Combine subtree embeddings
        std::vector<std::vector<int>> embedding(n);
        for (int i = 0, sz = reachable_pos.size(); i < sz; ++i) {
            const int v = reachable_pos[i];
            const int d = distances_pos[i];
            embedding[v] = embedding_pos[new_index[v]];
            embedding[v].push_back(d);
        }
        for (int i = 1, sz = reachable_neg.size(); i < sz; ++i) {
            const int v = reachable_neg[i];
            const int d = distances_neg[i];
            embedding[v] = embedding_neg[-new_index[v]];
            embedding[v].push_back(d);
            for (int& x : embedding[v])
                x = -x;
        }
        return embedding;
    }
}
std::vector<std::vector<int>> compute_frechet_embedding(const std::vector<std::pair<int, int>>& edges) {
    int n = 0;
    for (const std::pair<int, int>& edge : edges)
        n = std::max(n, std::max(edge.first, edge.second));
    n += 1;
    assert(n == (int)edges.size()+1);
    frechet_::global_adj.assign(n, {});
    frechet_::subtree_size.assign(n, 0);
    for (const std::pair<int, int>& edge : edges) {
        frechet_::global_adj[edge.first].push_back(edge.second);
        frechet_::global_adj[edge.second].push_back(edge.first);
    }
    return frechet_::compute_embedding(frechet_::global_adj);
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
