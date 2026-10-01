// Adversarial vertex / edge orderings of a few base graphs (N = 60000 each):
//   base: Moebius ladder (No), K3,3 subdivision in a deep tree (No), triangulation subset (Yes), 2-tree (Yes)
//   order: natural labels with edges sorted; and DFS labels with edges reversed and flipped
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

Graph ordered_graph(Random& gen, int base, int order) {
	const int n = 60000;
	Graph g;
	if (base == 0) g = moebius_ladder(n);
	else if (base == 1) {
		Graph sub = subdivide_edges(gen, complete_bipartite(3, 3), 300, 1000);
		g = random_deep_tree(gen, n - sub.n, 2);
		int off = g.add_graph(sub);
		int sub_v = off + gen.uniform(0, sub.n - 1), host_v = gen.uniform(0, off - 1);
		g.add_edge(host_v, sub_v);
	} else if (base == 2) g = random_edge_subset(gen, random_triangulation(gen, n, int64_t(3) * n), 0.6);
	else g = random_two_tree(gen, n);

	if (order >= 2) {
		auto label = bfs_order(g, gen, order == 3);
		for (auto& [u, v] : g.edges) { u = label[u]; v = label[v]; }
	}
	for (auto& [u, v] : g.edges) if (u > v) std::swap(u, v);
	std::sort(g.edges.begin(), g.edges.end());
	if (order == 1 || order == 3) std::reverse(g.edges.begin(), g.edges.end());
	if (order == 3) for (auto& [u, v] : g.edges) std::swap(u, v);
	return g;
}

int main(int, char* argv[]) {
	Random gen(atoll(argv[1]));
	std::vector<Graph> gs;
	for (int base = 0; base < 4; base++) for (int order : {0, 3}) gs.push_back(ordered_graph(gen, base, order));
	print_graphs(gen, gs, false, false, false);
	return 0;
}
