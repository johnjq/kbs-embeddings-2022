#!/usr/bin/env python3
from cProfile import label
from curses import raw
import statistics
import pprint
import numpy as np
import matplotlib.pyplot as plt
from sortedcontainers import SortedDict

def parse_line(line):
    cols = line.strip().split(' ')

    return {
        'input': int(cols[3]),
        'solution': cols[0],
        'runtime': float(cols[2]),
        'dimensionality': int(cols[4]),
    }

def main():
    with open("raw_results_for_benchmark_instances.txt", "r") as f:
        raw_lines = f.readlines()
    data = list(map(parse_line, raw_lines))
    process(data)

def  process(data):
    all_generators = set()
    plt.clf()
    plt.rc('font', size=8)
    x1 = []
    x2 = []
    x3 = []
    x4 = []
    y1 = []
    y2 = []
    y3 = []
    y4 = []
    t1 = []
    t2 = []
    t3 = []
    t4 = []
    print(data)
    
    for d in sorted(data, key=lambda e: (-e['input'], e['dimensionality'])):
        
        if d['solution'] == 'sol-Explicit':
            x1.append(d['input'])
            y1.append(d['dimensionality'])
            t1.append(d['runtime'])

        if d['solution'] == 'sol-ExplicitOpt':
            x2.append(d['input'])
            y2.append(d['dimensionality']) 
            t2.append(d['runtime'])
        
        if d['solution'] == 'sol-List':
            x3.append(d['input'])
            y3.append(d['dimensionality'])
            t3.append(d['runtime'])

        if d['solution'] == 'sol-ListOpt':
            x4.append(d['input'])
            y4.append(d['dimensionality']) 
            t4.append(d['runtime'])

    plt.plot(x1,y1,"-x",label='sol1',color='#0000AF')
    plt.plot(x2,y2,"-o",label='sol1-greedy',color='#ADD8E6')
    plt.ylabel('dimensionality')
    plt.xlabel('size')
    plt.xscale('log')
    plt.legend()
    plt.savefig('fig_benchmark_dimensionality.png', dpi=400)

    plt.clf()

    plt.plot(x1,t1,"-x",label='sol1',color='#0000AF')
    plt.plot(x2,t2,"-o",label='sol1-greedy',color='#ADD8E6')
    plt.plot(x3,t3,"-+",label='sol2',color='#AF0000')
    plt.plot(x4,t4,"-s",label='sol2-greedy',color='#FF8888')
    plt.ylabel('runtime (s)')
    plt.xlabel('size')
    #plt.xscale('log')
    plt.legend()
    plt.savefig('fig_benchmark_runtime.png', dpi=400)
    plt.show()

if __name__ == '__main__':
    main()
