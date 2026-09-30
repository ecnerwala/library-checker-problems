## @{keyword.statement}

@{lang.en}
This problem has $T$ cases.

You are given a simple undirected graph with $N$ vertices and $M$ edges. The $i$-th edge connects vertices $a_i$ and $b_i$. The graph is not necessarily connected.

Determine whether the graph is planar, i.e. whether it can be drawn in the plane with no two edges crossing. If it is, output a planar embedding as a rotation system: for each vertex, the cyclic order of its neighbors around it in such a drawing.
@{lang.ja}
この問題は $T$ ケースあります．

$N$ 頂点 $M$ 辺の単純無向グラフが与えられます．$i$ 番目の辺は頂点 $a_i$ と頂点 $b_i$ を結んでいます．このグラフは連結とは限りません．

このグラフが平面グラフであるか（辺が交差しないように平面に描けるか）判定し，平面グラフならばその平面埋め込みを回転系（rotation system）として出力してください．すなわち，各頂点について，その描画において隣接頂点がその頂点の周りに現れる巡回順序を出力してください．
@{lang.end}

## @{keyword.constraints}

@{lang.en}

- $@{param.T_MIN} \leq T \leq @{param.T_MAX}$
- $@{param.N_MIN} \leq N \leq @{param.N_MAX}$
- $@{param.M_MIN} \leq M \leq @{param.M_MAX}$
- $0 \leq a_i, b_i \lt N$
- $a_i \neq b_i$
- $\lbrace a_i, b_i \rbrace \neq \lbrace a_j, b_j \rbrace$ for $i \neq j$
- The sum of $N$ over all test cases does not exceed $@{param.N_MAX}$.
- The sum of $M$ over all test cases does not exceed $@{param.M_MAX}$.

@{lang.ja}

- $@{param.T_MIN} \leq T \leq @{param.T_MAX}$
- $@{param.N_MIN} \leq N \leq @{param.N_MAX}$
- $@{param.M_MIN} \leq M \leq @{param.M_MAX}$
- $0 \leq a_i, b_i \lt N$
- $a_i \neq b_i$
- $i \neq j$ ならば $\lbrace a_i, b_i \rbrace \neq \lbrace a_j, b_j \rbrace$
- 全てのテストケースに対する $N$ の総和は $@{param.N_MAX}$ を超えない．
- 全てのテストケースに対する $M$ の総和は $@{param.M_MAX}$ を超えない．

@{lang.end}

## @{keyword.input}

```
$T$
$N$ $M$
$a_0$ $b_0$
$a_1$ $b_1$
:
$a_{M - 1}$ $b_{M - 1}$
$N$ $M$
$a_0$ $b_0$
$a_1$ $b_1$
:
$a_{M - 1}$ $b_{M - 1}$
:
```

## @{keyword.output}

@{lang.en}
For each case, if the graph is not planar, print `No`.

Otherwise print `Yes`, followed by $N$ lines. The $v$-th line ($0 \leq v \lt N$) must contain the $\deg(v)$ neighbors of vertex $v$ in the cyclic order in which they appear around $v$ in some planar drawing of the graph (all vertices must use the same rotational direction within each connected component). The line for an isolated vertex is empty.

```
Yes
$c_{0, 0}$ $c_{0, 1}$ ... $c_{0, \deg(0) - 1}$
$c_{1, 0}$ $c_{1, 1}$ ... $c_{1, \deg(1) - 1}$
:
$c_{N - 1, 0}$ $c_{N - 1, 1}$ ... $c_{N - 1, \deg(N - 1) - 1}$
```

If there are multiple valid embeddings, any of them is accepted.
@{lang.ja}
各ケースについて，グラフが平面グラフでなければ `No` を出力してください．

平面グラフならば `Yes` を出力し，続けて $N$ 行を出力してください．$v$ 行目（$0 \leq v \lt N$）には，頂点 $v$ の $\deg(v)$ 個の隣接頂点を，ある平面描画において頂点 $v$ の周りに現れる巡回順序で出力してください（各連結成分の中では，全ての頂点で同じ回転方向を用いてください）．孤立点の行は空行です．

```
Yes
$c_{0, 0}$ $c_{0, 1}$ ... $c_{0, \deg(0) - 1}$
$c_{1, 0}$ $c_{1, 1}$ ... $c_{1, \deg(1) - 1}$
:
$c_{N - 1, 0}$ $c_{N - 1, 1}$ ... $c_{N - 1, \deg(N - 1) - 1}$
```

条件を満たす埋め込みが複数ある場合，どれを出力しても正解となります．
@{lang.end}

## @{keyword.sample}

@{example.example_00}

@{example.example_01}
