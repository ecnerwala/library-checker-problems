// Model solution: planar embedding by the Boyer-Myrvold edge addition
// algorithm, using John M. Boyer's reference implementation, the Edge
// Addition Planarity Suite, vendored unmodified (tag Version_5.1.0.0) in
// edge-addition-planarity-suite/ -- see the README.md there for the list
// of files and the adaptations made in this file.
//
// The library's .c files are compiled as C++ in this single translation unit.
// Distributed under the BSD-3-Clause license, see
// edge-addition-planarity-suite/LICENSE.TXT.

#include "edge-addition-planarity-suite/c/graphLib/lowLevelUtils/apiutils.c"
#include "edge-addition-planarity-suite/c/graphLib/lowLevelUtils/listcoll.c"
#include "edge-addition-planarity-suite/c/graphLib/lowLevelUtils/stack.c"
#include "edge-addition-planarity-suite/c/graphLib/io/strbuf.c"
#include "edge-addition-planarity-suite/c/graphLib/extensionSystem/graphExtensions.c"
#include "edge-addition-planarity-suite/c/graphLib/graph.c"
#include "edge-addition-planarity-suite/c/graphLib/graphDFSUtils.c"
// _gp_EmbedFlagsValid() passes `(void *)&context` to
// gp_FindExtension(graphP, int, void **): implicit in C, ill-formed in C++.
#define gp_FindExtension(theGraph, moduleID, pContext) \
    gp_FindExtension(theGraph, moduleID, static_cast<void **>(pContext))
#include "edge-addition-planarity-suite/c/graphLib/planarityRelated/graphEmbed.c"
#undef gp_FindExtension
#include "edge-addition-planarity-suite/c/graphLib/planarityRelated/graphNonplanar.c"
#include "edge-addition-planarity-suite/c/graphLib/planarityRelated/graphIsolator.c"
#include "edge-addition-planarity-suite/c/graphLib/planarityRelated/graphOuterplanarObstruction.c"
#include "edge-addition-planarity-suite/c/graphLib/planarityRelated/graphOuterplanarity_Extensions.c"
#include "edge-addition-planarity-suite/c/graphLib/planarityRelated/graphPlanarity_Extensions.c"
#include "edge-addition-planarity-suite/c/graphLib/planarityRelated/graphPlanarity_Faces.c"
#include "edge-addition-planarity-suite/c/graphLib/planarityRelated/graphTests.c"

// Referenced by graph.c / graphEmbed.c but defined in files that are not
// vendored (the drawing and K_{2,3}/K_{3,3}/K_4 extensions and graph I/O).
// None of them is reached by gp_Embed(EMBEDFLAGS_PLANAR).
int DRAWPLANAR_ID = 0;
int K23SEARCH_ID = 0;
int K33SEARCH_ID = 0;
int K4SEARCH_ID = 0;
int _ReadPostprocess(graphP, char *) { return OK; }
int _WritePostprocess(graphP, char **) { return OK; }


#include <string>

namespace {

char inbuf[1 << 16];
int inbuf_len = 0, inbuf_pos = 0;
inline int read_char() {
    if (inbuf_pos == inbuf_len) {
        inbuf_len = int(fread(inbuf, 1, sizeof(inbuf), stdin));
        inbuf_pos = 0;
        if (inbuf_len <= 0) return -1;
    }
    return inbuf[inbuf_pos++];
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

} // namespace

int main() {
    int T = read_int();
    std::string out;
    while (T--) {
        int N = read_int();
        int M = read_int();
        if (N >= 3 && (long long)M > 3LL * N - 6) {
            for (int i = 0; i < 2 * M; i++) read_int();
            out += "No\n";
            continue;
        }
        // The library uses 1-based vertex indices (USE_1BASEDARRAYS).
        graphP g = gp_New();
        if (gp_EnsureVertexCapacity(g, N) != OK) return 1;
        for (int i = 0; i < M; i++) {
            int a = read_int();
            int b = read_int();
            if (gp_AddEdge(g, a + 1, 0, b + 1, 0) != OK) return 1;
        }
        int r = gp_Embed(g, EMBEDFLAGS_PLANAR);
        if (r == OK) {
            // gp_Embed leaves the vertices in DFS order; restore the input numbering.
            if (gp_SortVertices(g) != OK) return 1;
            out += "Yes\n";
            for (int v = gp_LowerBoundVertices(g); v < gp_UpperBoundVertices(g); v++) {
                bool first = true;
                for (int e = gp_GetFirstEdge(g, v); gp_IsEdge(g, e); e = gp_GetNextEdge(g, e)) {
                    if (!first) out += ' ';
                    first = false;
                    append_int(out, gp_GetNeighbor(g, e) - 1);
                }
                out += '\n';
            }
        } else if (r == NONEMBEDDABLE) {
            out += "No\n";
        } else {
            return 1;
        }
        gp_Free(&g);
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
