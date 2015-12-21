#!/usr/bin/env python3
from os import EX_USAGE, EX_OK
from networkx import DiGraph, shortest_path, NetworkXNoPath
from itertools import islice, tee

nwise = lambda g,n=2: zip(*(islice(g,i,None) for i,g in enumerate(tee(g,n))))

if __name__ == '__main__':
    from sys import argv
    if len(argv) != 5:
        print('USAGE: {} input.dat [fare|route] DEPARTURE ARRIVAL'.format(argv[0]))
        exit(EX_USAGE)

    G = DiGraph()

    filename = argv[1]
    with open(filename) as f:
        for dep, arr, fare in (x.strip().split('\t') for x in f if x.strip()):
            G.add_edge(dep, arr, {'fare': int(fare)})

    departure, arrival = argv[3], argv[4]
    try:
        path = shortest_path(G, departure, arrival, weight='fare')
    except (KeyError, NetworkXNoPath) as e:
        print('No path from {} to {}'.format(departure, arrival))
        exit(EX_OK)

    fare = sum(G[x][y]['fare'] for x, y in nwise(path))

    if argv[2] == 'route':
        print('There is a route from {} to {}'.format(departure, arrival))
    elif argv[2] == 'fare':
        print('The cheapest fare from {} to {} is ${:.2f}'.format(departure, arrival, fare))
