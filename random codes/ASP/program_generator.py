from igraph import *
from math import inf
import random
random.seed()
N = 100
p = 0.02


def random_graph(n, p):
    g = Graph()
    g.add_vertex()
    for i in range(1, n):
        newEdgeCnt = 0
        g.add_vertex()
        for j in range(i):
            if random.random() < p:
                g.add_edge(i, j)
                newEdgeCnt += 1
                #print("node {} connect to node {}".format(i, j))
        if newEdgeCnt == 0:
            pick = random.randint(0, i-1)
            #print("node {} forced to connect to node {}".format(i, pick))
            g.add_edge(i, pick)
    # print(g)
    return g


def simul_BGP(g):
    g.vs["newRoutes"] = [{} for i in range(len(g.vs))]
    g.vs["routes"] = [{} for i in range(len(g.vs))]
    g.vs["shortestRoute"] = [[inf]*len(g.vs) for i in range(len(g.vs))]
    g.vs[0]["newRoutes"][0] = []
    propagate = True
    while propagate:
        propagate = False
        for i in range(len(g.vs)):
            ks = [*g.vs[i]["newRoutes"]]
            sr = g.vs[i]["shortestRoute"]
            advertise = False
            for k in ks:
                r = g.vs[i]["newRoutes"].pop(k)
                g.vs[i]["routes"][k] = r
                if (len(r) < len(sr)) or (len(r) == len(sr) and r < sr):
                    sr = r
                    advertise = True
            if advertise:
                g.vs[i]["shortestRoute"] = sr
                for e in g.adjacent(i):
                    if g.es[e].source == i:
                        n = g.es[e].target
                    else:
                        n = g.es[e].source
                    g.vs[n]["newRoutes"][i] = [i]+sr
                    #print("node {} advertised route {} to node {}".format(i, [i]+sr, n))
                    propagate = True


def program_generator(g, fileName):
    with open(fileName, 'w') as f:
        for v in range(1, len(g.vs)):
            routes = list(g.vs[v]["routes"].values())
            routes.sort()
            routes.sort(key=lambda r: len(r))
            assert routes[0] == g.vs[v]["shortestRoute"]
            routes = [tuple(r) for r in routes if v not in r]  # filter out loop routes
            f.write("% for node {}\n".format(v))
            if len(routes[0]) == 1:
                f.write("route({}).\n".format((v,)+routes[0]))
            for i in range(len(routes)):
                f.write("best({0}) :- route({0})".format((v,)+routes[i]))
                for j in range(i):
                    f.write(", not route({})".format((v,)+routes[j]))
                f.write(".\n")
            for r in routes[1:]:
                f.write("route({}) :- best({}).\n".format((v,)+r, r))  # get route from neighbor
            f.write("\n")


g = random_graph(N, p)
simul_BGP(g)
for i in range(len(g.vs)):
    print("vertex {} has {} routes and the shortest route is :{}".format(i, len(g.vs[i]["routes"]), g.vs[i]["shortestRoute"]))
program_generator(g, "test.txt")
