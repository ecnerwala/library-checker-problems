// WRONG: repeatedly deletes vertices of degree <= 1 and smooths vertices of
// degree 2 (dropping parallel edges), then answers No iff some connected
// component of the reduced graph is exactly K5 or K3,3. This catches
// subdivisions of K5 / K3,3 with pendant trees, but not graphs that only
// contain them as minors (Petersen, Wagner, Moebius ladders, K6, ...).
#include <cstdio>
#include <vector>
#include <set>
#include <algorithm>

int main() {
	int N, M;
	if (scanf("%d %d", &N, &M) != 2) return 1;
	std::vector<std::set<int>> adj(N);
	for (int i = 0; i < M; i++) {
		int a, b;
		scanf("%d %d", &a, &b);
		adj[a].insert(b);
		adj[b].insert(a);
	}
	std::vector<char> alive(N, 1);
	std::vector<int> queue;
	for (int v = 0; v < N; v++) if (adj[v].size() <= 2) queue.push_back(v);
	while (!queue.empty()) {
		int v = queue.back(); queue.pop_back();
		if (!alive[v] || adj[v].size() > 2) continue;
		alive[v] = 0;
		std::vector<int> nb(adj[v].begin(), adj[v].end());
		for (int w : nb) adj[w].erase(v);
		adj[v].clear();
		if (nb.size() == 2) {
			int a = nb[0], b = nb[1];
			if (!adj[a].count(b)) {
				adj[a].insert(b);
				adj[b].insert(a);
			}
		}
		for (int w : nb) if (adj[w].size() <= 2) queue.push_back(w);
	}
	// Examine components of the remaining graph.
	std::vector<int> color(N, -1);
	bool ok = true;
	for (int r = 0; r < N && ok; r++) {
		if (!alive[r] || color[r] != -1) continue;
		std::vector<int> comp;
		std::vector<int> stk{r};
		color[r] = 0;
		bool bipartite = true;
		while (!stk.empty()) {
			int v = stk.back(); stk.pop_back();
			comp.push_back(v);
			for (int w : adj[v]) {
				if (color[w] == -1) { color[w] = color[v] ^ 1; stk.push_back(w); }
				else if (color[w] == color[v]) bipartite = false;
			}
		}
		long long m = 0;
		bool all4 = true, all3 = true;
		for (int v : comp) {
			m += int(adj[v].size());
			if (adj[v].size() != 4) all4 = false;
			if (adj[v].size() != 3) all3 = false;
		}
		m /= 2;
		if (comp.size() == 5 && m == 10 && all4) ok = false;  // K5
		if (comp.size() == 6 && m == 9 && all3 && bipartite) ok = false;  // K3,3
	}
	puts(ok ? "Yes" : "No");
	return 0;
}
