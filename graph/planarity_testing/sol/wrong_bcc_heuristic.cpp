// WRONG: splits the graph into biconnected components and accepts iff every
// block with n >= 3 vertices satisfies m <= 3n - 6 and has a vertex of degree
// <= 5 (within the block). Fails e.g. on the Petersen graph or Moebius ladders.
#include <cstdio>
#include <vector>
#include <algorithm>

int main() {
	int N, M;
	if (scanf("%d %d", &N, &M) != 2) return 1;
	std::vector<int> ea(M), eb(M);
	std::vector<int> start(N + 1, 0);
	for (int i = 0; i < M; i++) {
		scanf("%d %d", &ea[i], &eb[i]);
		start[ea[i] + 1]++;
		start[eb[i] + 1]++;
	}
	for (int i = 0; i < N; i++) start[i + 1] += start[i];
	std::vector<int> adj(2 * M);
	{
		std::vector<int> pos(start.begin(), start.end() - 1);
		for (int i = 0; i < M; i++) {
			adj[pos[ea[i]]++] = i;
			adj[pos[eb[i]]++] = i;
		}
	}
	std::vector<int> disc(N, -1), low(N, 0), it(N), par_edge(N, -1);
	std::vector<int> estack;
	std::vector<int> stk;
	std::vector<int> mark(N, -1), deg(N, 0);
	int timer = 0, block_id = 0;
	bool ok = true;

	auto process_block = [&](std::vector<int>& edges) -> void {
		block_id++;
		std::vector<int> verts;
		for (int e : edges) {
			for (int v : {ea[e], eb[e]}) {
				if (mark[v] != block_id) { mark[v] = block_id; deg[v] = 0; verts.push_back(v); }
				deg[v]++;
			}
		}
		long long n = int(verts.size()), m = int(edges.size());
		if (n >= 3) {
			if (m > 3 * n - 6) ok = false;
			int mn = 1 << 30;
			for (int v : verts) mn = std::min(mn, deg[v]);
			if (mn > 5) ok = false;
		}
	};

	for (int r = 0; r < N; r++) {
		if (disc[r] != -1) continue;
		disc[r] = low[r] = timer++;
		it[r] = start[r];
		stk.push_back(r);
		while (!stk.empty()) {
			int v = stk.back();
			if (it[v] < start[v + 1]) {
				int e = adj[it[v]++];
				if (e == par_edge[v]) continue;
				int w = ea[e] ^ eb[e] ^ v;
				if (disc[w] == -1) {
					disc[w] = low[w] = timer++;
					it[w] = start[w];
					par_edge[w] = e;
					estack.push_back(e);
					stk.push_back(w);
				} else if (disc[w] < disc[v]) {
					estack.push_back(e);
					low[v] = std::min(low[v], disc[w]);
				}
			} else {
				stk.pop_back();
				if (!stk.empty()) {
					int u = stk.back();
					low[u] = std::min(low[u], low[v]);
					if (low[v] >= disc[u]) {
						std::vector<int> edges;
						while (true) {
							int e = estack.back(); estack.pop_back();
							edges.push_back(e);
							if (e == par_edge[v]) break;
						}
						process_block(edges);
					}
				}
			}
		}
	}
	puts(ok ? "Yes" : "No");
	return 0;
}
