#!/usr/bin/env python3
import statistics
import pprint
import numpy as np
import matplotlib.pyplot as plt

def parse_line(line):
    cols = line.strip().split(' ')
    return {
        'generator': cols[0][4:],
        'input': int(cols[1]),
        'solution': cols[2],
        'runtime': float(cols[3]),
        'dimensionality': int(cols[4]),
    }

def main():
    with open("raw_results_for_synthetic_instances.txt", "r") as f:
        raw_lines = f.readlines()
    raw_data = list(map(parse_line, raw_lines))
    data = simplify(raw_data)
    process(data)

def simplify(raw_data):
    # Categorize data
    categorized_data = {}
    for row in raw_data:
        key = (row['generator'], row['input'], row['solution'])
        if not key in categorized_data:
            categorized_data[key] = []
        categorized_data[key].append({'runtime': row['runtime'], 'dimensionality': row['dimensionality']})
    # Compute statistics
    data = {}
    for key, samples in categorized_data.items():
        runtimes = [s['runtime'] for s in samples]
        dimensionalities = [s['dimensionality'] for s in samples]
        data[key] = {
            'runtime_mean': statistics.mean(runtimes),
            'runtime_stdev': statistics.stdev(runtimes),
            'dimensionality_mean': statistics.mean(dimensionalities),
            'dimensionality_stdev': statistics.stdev(dimensionalities),
        }
    return data

def process(data):
    # Retrive groups
    all_generators = set()
    all_inputs = set()
    all_solutions = set()
    for key in data:
        all_generators.add(key[0])
        all_inputs.add(key[1])
        all_solutions.add(key[2])
    all_generators = ["random", "star", "binary", "ternary", "quaternary", "bamboo", "caterpillar", "banana"]
    all_solutions = ["sol-Explicit", "sol-ExplicitOpt", "sol-List", "sol-ListOpt"]
    rename_solutions = {"sol-Explicit": "sol1", "sol-ExplicitOpt": "sol1-greedy", "sol-List": "sol2", "sol-ListOpt": "sol2-greedy"}
    # Print results
    print("*** Results")
    for key, value in data.items():
        print("%-37s %.4f (%.4f)       %.4f (%.4f)" % (key, value['runtime_mean'], value['runtime_stdev'], value['dimensionality_mean'], value['dimensionality_stdev']))
    # Print dimensionality reductions (vanilla -> greedy)
    print()
    print("*** Dimensionality reductions (vanilla -> greedy)")
    n = 100000
    for gen in all_generators:
        dimensionality_list = data[(gen, n, 'sol-List')]['dimensionality_mean']
        dimensionality_listOpt = data[(gen, n, 'sol-ListOpt')]['dimensionality_mean']
        print("%s: reduction of %.1f%% (%.2f to %.2f)" % (gen, 100*(dimensionality_list - dimensionality_listOpt) / dimensionality_list, dimensionality_list, dimensionality_listOpt))
    # Print runtime reductions (explicit -> implicit)
    print()
    print("*** Runtime reductions (explicit -> implicit)")
    for gen in all_generators:
        runtime_list = data[(gen, n, 'sol-List')]['runtime_mean']
        runtime_listOpt = data[(gen, n, 'sol-ListOpt')]['runtime_mean']
        runtime_explicit = data[(gen, n, 'sol-Explicit')]['runtime_mean']
        runtime_explicitOpt = data[(gen, n, 'sol-ExplicitOpt')]['runtime_mean']
        print("%s (vanilla): reduction of %.1f%% (%.2f to %.2f)" % (gen, 100*(runtime_explicit - runtime_list) / runtime_explicit, runtime_explicit, runtime_list))
        print("%s (greedy): reduction of %.1f%% (%.2f to %.2f)" % (gen, 100*(runtime_explicitOpt - runtime_listOpt) / runtime_explicitOpt, runtime_explicitOpt, runtime_listOpt))
    # Print runtime reductions (explicit, vanilla -> greedy)
    print()
    print("*** Runtime reductions (explicit, vanilla -> greedy)")
    for gen in all_generators:
        runtime_explicit = data[(gen, n, 'sol-Explicit')]['runtime_mean']
        runtime_explicitOpt = data[(gen, n, 'sol-ExplicitOpt')]['runtime_mean']
        print("%s: reduction of %.1f%% (%.2f to %.2f)" % (gen, 100*(runtime_explicit - runtime_explicitOpt) / runtime_explicit, runtime_explicit, runtime_explicitOpt))
    # Print runtime reductions (implicit, vanilla -> greedy)
    print()
    print("*** Runtime reductions (implicit, vanilla -> greedy)")
    for gen in all_generators:
        runtime_list = data[(gen, n, 'sol-List')]['runtime_mean']
        runtime_listOpt = data[(gen, n, 'sol-ListOpt')]['runtime_mean']
        print("%s: reduction of %.1f%% (%.2f to %.2f)" % (gen, 100*(runtime_list - runtime_listOpt) / runtime_list, runtime_list, runtime_listOpt))
    # Compare solutions
    colors = {'sol-Explicit': '#0000AF', 'sol-ExplicitOpt': '#ADD8E6', 'sol-List': '#AF0000', 'sol-ListOpt': '#FF8888'}
    input = 100000
    bar_width = 1 / (1 + len(colors))
    pos = np.arange(len(all_generators))
    plt.clf()
    plt.rc('font', size=8)
    for sol in all_solutions:
        values = []
        errors = []
        for gen in all_generators:
            value = data[(gen, input, sol)]
            values.append(value['runtime_mean'])
            errors.append(value['runtime_stdev'] * 2)
        plt.bar(pos, values, width = bar_width, color = colors[sol], edgecolor = 'black', yerr=errors, capsize=7, label=rename_solutions[sol])
        pos = [x + bar_width for x in pos]
    plt.xticks([r + (len(colors)-1)/2*bar_width for r in range(len(all_generators))], all_generators)
    plt.ylabel('runtime (s)')
    plt.legend()
    plt.savefig('fig_synthetic_runtime.png', dpi=400)
    # Compare generators
    colors = {'sol-Explicit': '#0000AF', 'sol-ExplicitOpt': '#ADD8E6'}
    input = 100000
    bar_width = 1 / (1 + len(colors))
    pos = np.arange(len(all_generators))
    plt.clf()
    for sol in colors.keys():
        values = []
        errors = []
        for gen in all_generators:
            value = data[(gen, input, sol)]
            values.append(value['dimensionality_mean'])
            errors.append(value['dimensionality_stdev'] * 2)
        plt.bar(pos, values, width = bar_width, color = colors[sol], edgecolor = 'black', yerr=errors, capsize=7, label=rename_solutions[sol])
        pos = [x + bar_width for x in pos]
    plt.xticks([r + (len(colors)-1)/2*bar_width for r in range(len(all_generators))], all_generators)
    plt.ylabel('dimensionality')
    plt.legend()
    plt.savefig('fig_synthetic_dimensionality.png', dpi=400)

if __name__ == '__main__':
    main()
