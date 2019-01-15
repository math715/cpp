# A*搜索算法（A Star Search Algorithm）

A*算法主要用于在二维平面上寻找两个点之间的最短路径。在从起始点到目标点的过程中有很多个状态空间，DFS和BFS没有任何启发策略所以穷举所有的状 态空间，不适合仅需对局部进行搜索的应用。启发式搜索的关键在于：当前节点在选择下一步节点的时候，可以通过一个启发函数进行选择，选择到达终点代价最小 的节点作为下一步节点。A*的启发函数构造为：

$f(n)=g(n)+h(n)$

f(n)是可选的下一个节点的代 价，g(n)表示从start点到n点的代价，h(n)表示从n点到end点的代价；但是h(n)是无法预知的，（如果将BFS看作一种A*的话，其 h(n)恒定为0，g(n)表示到起始点的层数）所以A*算法将当n点到end点的直线距离作为h(n)的估值，显然只能无限接近实际最优解。

A*算法维护两张表OPEN和CLOSED，OPEN保存所有可探测但是还没有访问的节点，CLOSED保存素有已经访问的节点；启发函数就当前在 OPEN表中的节点进行排序，按照代价从低到高排列，每次选择代价最小的节点进行访问；当前访问的节点会根据簿记信息更新代价信息，并且将其直接连接的子 节点访问OPEN表中，最后将当前节点放入CLOSED表；A*算法开始于将start点放入OPEN表，结束于找到end点，或者OPEN表为 空，CLOSED表已经包含所有可访问节点。

程序实现中g(n)表示在抽象的状态空间中start点到n点的深度，h(n)表示在实际地图中n点所在地图位置到end点地图位置的直线距离，伪代码如下：



```java
function A*(start,goal)
     closedset := the empty set
                // closedset存储已经访问过的节点
     openset := {start}
                //openset存储已经探测到但还未访问的节点，初始化时放入start点
     came_from := the empty map
                //came_from存储最短路径
 
     g_score[start] := 0
                //g_score[u]存储当前当u点到start点的最小代价
     h_score[start] := heuristic_cost_estimate(start, goal)
                //h_score[u]存储当前u点到goal点的最小代价
     f_score[start] := g_score[start] + h_score[start]
                //f_score[u]存储当前u点在整个状态空间中的代价

     while openset is not empty
         current := the node in openset having the lowest f_score[] value
                //从当前openset中选取具有最小f_score的节点current
         if current = goal
             return reconstruct_path(came_from, came_from[goal])
                //判定如果是goal点，则结束
         remove current from openset
         add current to closedset
                //判定如果不是goal点，则将current点从openset中移到closedset
         for each neighbor in neighbor_nodes(current)
                //遍历current直接相连的节点neighbor
                //如果neighbor已经处理过一次（在closedset中），则跳过
             if neighbor in closedset
                 continue
             tentative_g_score := g_score[current] + dist_between(current,neighbor)
                //利用当前current点的g_score重新计算neighbor的g_score
             if neighbor not in openset
                //如果neighbor是新探测到的节点（没有在openset中），创建新节点
                 add neighbor to openset
                 h_score[neighbor] := heuristic_cost_estimate(neighbor, goal)
                 tentative_is_better := true
             else if tentative_g_score < g_score[neighbor]
                //如果neighbor已经探测到，并且当前g_socre[current]有更小的代价
                 tentative_is_better := true
             else
                 tentative_is_better := false
                 //更新neighbor的代价
             if tentative_is_better = true
                 came_from[neighbor] := current
                 g_score[neighbor] := tentative_g_score
                 f_score[neighbor] := g_score[neighbor] + h_score[neighbor]
 
     return failure
 
 function reconstruct_path(came_from, current_node)
     if came_from[current_node] is set
         p := reconstruct_path(came_from, came_from[current_node])
         return (p + current_node)
     else
         return current_node
```


引用:
https://www.cnblogs.com/leo-chen-2014/p/3756490.html


## Note:
问题1: OpenSet 采用multiset, 不是优先列队, Set
采用Mutliset的目的,让PointInfo按照F_score值,从小到大进行排序. 由于PointInfo可能在多个F_score一致的情况,造成Set无法使用.
priority_queue, 提供的功能太少了. 无法删除指定的Point_Info. 由于计算过程中, 需要不断更新的PointInfo的得分. 需要删除旧的PointInfo, 插入新的PointInfo; 
multiset, 根据Point查询PointInfo,需要遍历. 因此,增加HashMap方式,也就是UnorderedMap,方便查询Point是否在OpenSet中, 也方便获取PointInfo;

问题2: Search时,出现多条相同得分的路径, 无法给出来多条路径,选择其中一条路径做为最终的结果
由于根据节点相连方向顺序生成下一个节点,影响路径的选择. 此不支持多条路径输出