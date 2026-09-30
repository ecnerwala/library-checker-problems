// Wrong: answers Yes iff every connected component with n >= 3 vertices has
// m <= 3n - 6, and prints the neighbors in input order as the "embedding".
#include <cstdio>
#include <vector>
#include <string>
#include <numeric>

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
void append_int(std::string& out, int x) {
	char tmp[12];
	int len = 0;
	if (x == 0) tmp[len++] = '0';
	while (x > 0) { tmp[len++] = char('0' + x % 10); x /= 10; }
	while (len > 0) out += tmp[--len];
}

std::vector<int> par;
int find(int x) { while (par[x] != x) x = par[x] = par[par[x]]; return x; }

} // namespace

int main() {
	int T = read_int();
	std::string out;
	while (T--) {
		int N = read_int();
		int M = read_int();
		par.resize(N);
		std::iota(par.begin(), par.end(), 0);
		std::vector<std::vector<int>> adj(N);
		for (int i = 0; i < M; i++) {
			int a = read_int(), b = read_int();
			adj[a].push_back(b);
			adj[b].push_back(a);
			par[find(a)] = find(b);
		}
		std::vector<long long> n_c(N, 0), m_c(N, 0);
		for (int v = 0; v < N; v++) {
			n_c[find(v)]++;
			m_c[find(v)] += int(adj[v].size());
		}
		bool ok = true;
		for (int v = 0; v < N; v++) {
			if (find(v) == v && n_c[v] >= 3 && m_c[v] / 2 > 3 * n_c[v] - 6) ok = false;
		}
		if (!ok) {
			out += "No\n";
			continue;
		}
		out += "Yes\n";
		for (int v = 0; v < N; v++) {
			for (size_t i = 0; i < adj[v].size(); i++) {
				if (i) out += ' ';
				append_int(out, adj[v][i]);
			}
			out += '\n';
		}
	}
	fwrite(out.data(), 1, out.size(), stdout);
	return 0;
}
