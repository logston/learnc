#!/usr/bin/env python3
# vim: set fileencoding=utf-8 :

from itertools import combinations
from random import randrange
from os import EX_USAGE

if __name__ == '__main__':
    from sys import argv, exit

    if len(argv) != 2:
        print('USAGE: {} cities.dat'.format(argv[1]))
        exit(EX_USAGE)

    with open(argv[1]) as f:
        cities = [line.strip() for line in f.readlines() if line.strip()]

    for departure, arrival in combinations(cities, 2):
        print('{}\t{}\t{}'.format(departure, arrival, randrange(100, 1000)))
