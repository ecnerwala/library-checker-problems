// Independent cross-check: iterative left-right planarity test
// (U. Brandes, "The Left-Right Planarity Test", 2009), following the
// structure of networkx's networkx/algorithms/planarity.py.
#include <cstdio>
#include <vector>
#include <array>
#include <algorithm>
#include <cassert>

namespace {

char buf[1 << 16];
int buf_len = 0, buf_pos = 0;
inline int read_char() {
	if (buf_pos == buf_len) {
		buf_len = int(fread(buf, 1, sizeof(buf), stdin));
		buf_pos = 0;
		if (buf_len <= 0) return -1;
	}
	return buf[buf_pos++];
}
inline int read_int() {
	int c = read_char();
	while (c < '0' || c > '9') { if (c == -1) return 0; c = read_char(); }
	int x = 0;
	while (c >= '0' && c <= '9') { x = x * 10 + (c - '0'); c = read_char(); }
	return x;
}

struct Interval {
	int low = -1, high = -1;
	bool empty() const { return low == -1 && high == -1; }
};
struct ConflictPair {
	Interval L, R;
	int id;
};

struct LRPlanarity {
	int N, M;
	std::vector<std::array<int, 2>> edges;
	std::vector<int> adj_start, adj_edge; // undirected CSR: adj_edge holds edge ids
	std::vector<int> height, parent_edge;
	std::vector<int> from, to;
	std::vector<char> oriented;
	std::vector<int> lowpt, lowpt2, nesting_depth;
	std::vector<int> out_start, out_edge; // DFS-oriented out-edges, sorted by nesting_depth
	std::vector<int> ref, lowpt_edge, stack_bottom;
	std::vector<ConflictPair> S;
	int next_id = 0;

	LRPlanarity(int N_, std::vector<std::array<int, 2>> edges_) : N(N_), M(int(edges_.size())), edges(std::move(edges_)) {}

	int top_id() const { return S.empty() ? -1 : S.back().id; }

	void orient() {
		adj_start.assign(N + 1, 0);
		for (auto [u, v] : edges) { adj_start[u + 1]++; adj_start[v + 1]++; }
		for (int i = 0; i < N; i++) adj_start[i + 1] += adj_start[i];
		adj_edge.resize(2 * M);
		{
			std::vector<int> pos(adj_start.begin(), adj_start.end() - 1);
			for (int e = 0; e < M; e++) {
				adj_edge[pos[edges[e][0]]++] = e;
				adj_edge[pos[edges[e][1]]++] = e;
			}
		}
		height.assign(N, -1);
		parent_edge.assign(N, -1);
		from.assign(M, -1); to.assign(M, -1);
		oriented.assign(M, 0);
		lowpt.assign(M, 0); lowpt2.assign(M, 0); nesting_depth.assign(M, 0);
		std::vector<int> out_cnt(N + 1, 0);
		std::vector<int> ind(N), pending(N, -1);
		std::vector<int> st;
		st.reserve(N);
		auto post = [&](int v, int ei) -> void {
			nesting_depth[ei] = 2 * lowpt[ei];
			if (lowpt2[ei] < height[v]) nesting_depth[ei] += 1;
			int e = parent_edge[v];
			if (e != -1) {
				if (lowpt[ei] < lowpt[e]) {
					lowpt2[e] = std::min(lowpt[e], lowpt2[ei]);
					lowpt[e] = lowpt[ei];
				} else if (lowpt[ei] > lowpt[e]) {
					lowpt2[e] = std::min(lowpt2[e], lowpt[ei]);
				} else {
					lowpt2[e] = std::min(lowpt2[e], lowpt2[ei]);
				}
			}
		};
		for (int r = 0; r < N; r++) {
			if (height[r] != -1) continue;
			height[r] = 0;
			ind[r] = adj_start[r];
			st.push_back(r);
			while (!st.empty()) {
				int v = st.back();
				if (pending[v] != -1) {
					post(v, pending[v]);
					pending[v] = -1;
					ind[v]++;
				}
				bool descended = false;
				while (ind[v] < adj_start[v + 1]) {
					int ei = adj_edge[ind[v]];
					if (oriented[ei]) { ind[v]++; continue; }
					int w = edges[ei][0] ^ edges[ei][1] ^ v;
					oriented[ei] = 1;
					from[ei] = v; to[ei] = w;
					out_cnt[v + 1]++;
					lowpt[ei] = lowpt2[ei] = height[v];
					if (height[w] == -1) {
						parent_edge[w] = ei;
						height[w] = height[v] + 1;
						ind[w] = adj_start[w];
						pending[v] = ei;
						st.push_back(w);
						descended = true;
						break;
					}
					lowpt[ei] = height[w];
					post(v, ei);
					ind[v]++;
				}
				if (!descended) st.pop_back();
			}
		}
		// Stable counting sort of oriented edges by nesting_depth, then bucket by source.
		std::vector<int> by_depth(M);
		{
			int D = 2 * N + 2;
			std::vector<int> cnt(D + 1, 0);
			for (int e = 0; e < M; e++) cnt[nesting_depth[e] + 1]++;
			for (int i = 0; i < D; i++) cnt[i + 1] += cnt[i];
			for (int e = 0; e < M; e++) by_depth[cnt[nesting_depth[e]]++] = e;
		}
		for (int i = 0; i < N; i++) out_cnt[i + 1] += out_cnt[i];
		out_start = out_cnt;
		out_edge.resize(M);
		{
			std::vector<int> pos(out_start.begin(), out_start.end() - 1);
			for (int e : by_depth) out_edge[pos[from[e]]++] = e;
		}
	}

	bool conflicting(const Interval& I, int b) const {
		return !I.empty() && lowpt[I.high] > lowpt[b];
	}
	int lowest(const ConflictPair& P) const {
		if (P.L.empty()) return lowpt[P.R.low];
		if (P.R.empty()) return lowpt[P.L.low];
		return std::min(lowpt[P.L.low], lowpt[P.R.low]);
	}

	bool add_constraints(int ei, int e) {
		ConflictPair P{Interval{}, Interval{}, next_id++};
		// merge return edges of e_i into P.R
		while (true) {
			assert(!S.empty());
			ConflictPair Q = S.back(); S.pop_back();
			if (!Q.L.empty()) std::swap(Q.L, Q.R);
			if (!Q.L.empty()) return false;
			if (lowpt[Q.R.low] > lowpt[e]) {
				if (P.R.empty()) P.R = Q.R;
				else ref[P.R.low] = Q.R.high;
				P.R.low = Q.R.low;
			} else {
				ref[Q.R.low] = lowpt_edge[e];
			}
			if (top_id() == stack_bottom[ei]) break;
		}
		// merge conflicting return edges of e_1, ..., e_{i-1} into P.L
		while (!S.empty() && (conflicting(S.back().L, ei) || conflicting(S.back().R, ei))) {
			ConflictPair Q = S.back(); S.pop_back();
			if (conflicting(Q.R, ei)) std::swap(Q.L, Q.R);
			if (conflicting(Q.R, ei)) return false;
			ref[P.R.low] = Q.R.high;
			if (Q.R.low != -1) P.R.low = Q.R.low;
			if (P.L.empty()) P.L = Q.L;
			else ref[P.L.low] = Q.L.high;
			P.L.low = Q.L.low;
		}
		if (!(P.L.empty() && P.R.empty())) S.push_back(P);
		return true;
	}

	void remove_back_edges(int e) {
		int u = from[e];
		while (!S.empty() && lowest(S.back()) == height[u]) S.pop_back();
		if (!S.empty()) {
			ConflictPair P = S.back(); S.pop_back();
			while (P.L.high != -1 && to[P.L.high] == u) P.L.high = ref[P.L.high];
			if (P.L.high == -1 && P.L.low != -1) {
				ref[P.L.low] = P.R.low;
				P.L.low = -1;
			}
			while (P.R.high != -1 && to[P.R.high] == u) P.R.high = ref[P.R.high];
			if (P.R.high == -1 && P.R.low != -1) {
				ref[P.R.low] = P.L.low;
				P.R.low = -1;
			}
			S.push_back(P);
		}
		if (lowpt[e] < height[u]) {
			int hl = S.back().L.high, hr = S.back().R.high;
			if (hl != -1 && (hr == -1 || lowpt[hl] > lowpt[hr])) ref[e] = hl;
			else ref[e] = hr;
		}
	}

	bool test() {
		ref.assign(M, -1);
		lowpt_edge.assign(M, -1);
		stack_bottom.assign(M, -1);
		S.clear();
		S.reserve(M);
		std::vector<int> ind(N), pending(N, -1);
		std::vector<int> st;
		st.reserve(N);
		auto integrate = [&](int v, int ei) -> bool {
			if (lowpt[ei] < height[v]) {
				int e = parent_edge[v];
				assert(e != -1);
				if (ei == out_edge[out_start[v]]) lowpt_edge[e] = lowpt_edge[ei];
				else if (!add_constraints(ei, e)) return false;
			}
			return true;
		};
		for (int r = 0; r < N; r++) {
			if (parent_edge[r] != -1) continue;
			ind[r] = out_start[r];
			st.push_back(r);
			while (!st.empty()) {
				int v = st.back();
				if (pending[v] != -1) {
					int ei = pending[v];
					pending[v] = -1;
					if (!integrate(v, ei)) return false;
					ind[v]++;
				}
				bool descended = false;
				while (ind[v] < out_start[v + 1]) {
					int ei = out_edge[ind[v]];
					int w = to[ei];
					stack_bottom[ei] = top_id();
					if (ei == parent_edge[w]) {
						ind[w] = out_start[w];
						pending[v] = ei;
						st.push_back(w);
						descended = true;
						break;
					}
					lowpt_edge[ei] = ei;
					S.push_back(ConflictPair{Interval{}, Interval{ei, ei}, next_id++});
					if (!integrate(v, ei)) return false;
					ind[v]++;
				}
				if (!descended) {
					if (parent_edge[v] != -1) remove_back_edges(parent_edge[v]);
					st.pop_back();
				}
			}
		}
		return true;
	}

	bool is_planar() {
		orient();
		return test();
	}
};

} // namespace

int main() {
	int N = read_int();
	int M = read_int();
	std::vector<std::array<int, 2>> edges(M);
	for (auto& e : edges) {
		e[0] = read_int();
		e[1] = read_int();
	}
	LRPlanarity lr(N, std::move(edges));
	puts(lr.is_planar() ? "Yes" : "No");
	return 0;
}
