## @{keyword.statement}

@{lang.en}
You are given a simple undirected graph with $N$ vertices and $M$ edges. The $i$-th edge connects vertices $a_i$ and $b_i$. The graph is not necessarily connected.

Determine whether the graph is planar, i.e. whether it can be drawn in the plane with no two edges crossing.
@{lang.ja}
$N$ 頂点 $M$ 辺の単純無向グラフが与えられます．$i$ 番目の辺は頂点 $a_i$ と頂点 $b_i$ を結んでいます．このグラフは連結とは限りません．

このグラフが平面グラフであるか（辺が交差しないように平面に描けるか）判定してください．
@{lang.end}

## @{keyword.constraints}

- $@{param.N_MIN} \leq N \leq @{param.N_MAX}$
- $@{param.M_MIN} \leq M \leq @{param.M_MAX}$
- $0 \leq a_i, b_i \lt N$
- $a_i \neq b_i$
- $\lbrace a_i, b_i \rbrace \neq \lbrace a_j, b_j \rbrace$ for $i \neq j$

## @{keyword.input}

```
$N$ $M$
$a_0$ $b_0$
$a_1$ $b_1$
:
$a_{M - 1}$ $b_{M - 1}$
```

## @{keyword.output}

@{lang.en}
Print `Yes` if the graph is planar, and `No` otherwise.
@{lang.ja}
グラフが平面グラフならば `Yes` を，そうでなければ `No` を出力してください．
@{lang.end}

## @{keyword.sample}

@{example.example_00}

@{example.example_01}

@{example.example_02}

@{example.example_03}

@{example.example_04}

@{example.example_05}
