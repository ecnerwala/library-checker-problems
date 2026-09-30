// WRONG: answers Yes iff every connected component with n >= 3 vertices
// satisfies m <= 3n - 6 (Euler's bound). Fails on sparse nonplanar graphs
// such as K3,3 subdivisions or the Petersen graph.
#include <cstdio>
#include <vector>
#include <numeric>

struct DSU {
	std::vector<int> p;
	explicit DSU(int n) : p(n) { std::iota(p.begin(), p.end(), 0); }
	int find(int x) { while (p[x] != x) x = p[x] = p[p[x]]; return x; }
	void unite(int a, int b) { a = find(a), b = find(b); if (a != b) p[a] = b; }
};

int main() {
	int N, M;
	if (scanf("%d %d", &N, &M) != 2) return 1;
	std::vector<int> a(M), b(M);
	DSU dsu(N);
	for (int i = 0; i < M; i++) {
		scanf("%d %d", &a[i], &b[i]);
		dsu.unite(a[i], b[i]);
	}
	std::vector<long long> nv(N, 0), ne(N, 0);
	for (int v = 0; v < N; v++) nv[dsu.find(v)]++;
	for (int i = 0; i < M; i++) ne[dsu.find(a[i])]++;
	bool ok = true;
	for (int v = 0; v < N; v++) {
		if (nv[v] >= 3 && ne[v] > 3 * nv[v] - 6) ok = false;
	}
	puts(ok ? "Yes" : "No");
	return 0;
}
