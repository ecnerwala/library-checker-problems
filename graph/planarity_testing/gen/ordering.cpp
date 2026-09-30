// Adversarial vertex / edge orderings of a few base graphs (N ~ 2 * 10^5):
//   base (seed / 4): Moebius ladder (No), K3,3 subdivision in a deep tree (No),
//                    triangulation subset (Yes), 2-tree (Yes)
//   order (seed % 4): 0: natural labels, edges sorted; 1: natural labels, edges reverse-sorted;
//                     2: BFS labels, edges sorted; 3: DFS labels, edges reversed and flipped
#include "planar_gen.h"
#include "../params.h"

std::vector<int> bfs_order(const Graph& g, Random& gen, bool dfs) {
	std::vector<std::vector<int>> adj(g.n);
	for (auto [u, v] : g.edges) { adj[u].push_back(v); adj[v].push_back(u); }
	std::vector<int> label(g.n, -1);
	int nxt = 0;
	std::vector<int> q;
	for (int s : gen.perm<int>(g.n)) {
		if (label[s] != -1) continue;
		q.assign(1, s); label[s] = nxt++;
		for (size_t i = 0; !q.empty() && (dfs || i < q.size()); ) {
			int v;
			if (dfs) { v = q.back(); q.pop_back(); }
			else v = q[i++];
			for (int w : adj[v]) if (label[w] == -1) { label[w] = nxt++; q.push_back(w); }
		}
		if (!dfs) q.clear();
	}
	return label;
}

int main(int, char* argv[]) {
	long long seed = atoll(argv[1]);
	Random gen(seed);
	int base = (seed / 4) % 4, order = seed % 4;
	Graph g;
	if (base == 0) g = moebius_ladder(200000);
	else if (base == 1) {
		Graph sub = subdivide_edges(gen, complete_bipartite(3, 3), 1000, 3000);
		g = random_deep_tree(gen, 200000 - sub.n, 2);
		int off = g.add_graph(sub);
		g.add_edge(gen.uniform(0, off - 1), off + gen.uniform(0, sub.n - 1));
	} else if (base == 2) g = random_edge_subset(gen, random_triangulation(gen, 200000, 600000), 0.6);
	else g = random_two_tree(gen, 200000);

	if (order >= 2) {
		auto label = bfs_order(g, gen, order == 3);
		for (auto& [u, v] : g.edges) { u = label[u]; v = label[v]; }
	}
	for (auto& [u, v] : g.edges) if (u > v) std::swap(u, v);
	std::sort(g.edges.begin(), g.edges.end());
	if (order == 1 || order == 3) std::reverse(g.edges.begin(), g.edges.end());
	if (order == 3) for (auto& [u, v] : g.edges) std::swap(u, v);
	assert(int(g.edges.size()) <= M_MAX);
	print_graph(gen, g, false, false, false);
	return 0;
}
