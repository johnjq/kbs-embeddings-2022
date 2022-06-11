#!/usr/bin/env bash
set -e

RAW_RESULTS_FILENAME="raw_results_for_benchmark_instances.txt"

INSTANCES_DIRECTORY="$1"
if [ -z "$INSTANCES_DIRECTORY" ]; then
    echo "Error: the instances directory must be passed as an argument"
    exit 1
fi
if ! [ -d "$INSTANCES_DIRECTORY" ]; then
    echo "Error: the instances directory does not exist ($INSTANCES_DIRECTORY)"
    exit 1
fi

echo "Instances directory: $INSTANCES_DIRECTORY"

echo "=> Compiling solutions..."
g++ -std=c++11 -O2 solutions/sol-List.cpp -o solutions/sol-List.exe
g++ -std=c++11 -O2 solutions/sol-ListOpt.cpp -o solutions/sol-ListOpt.exe
g++ -std=c++11 -O2 solutions/sol-Explicit.cpp -o solutions/sol-Explicit.exe
g++ -std=c++11 -O2 solutions/sol-ExplicitOpt.cpp -o solutions/sol-ExplicitOpt.exe

echo "=> Running..."
echo -n > "$RAW_RESULTS_FILENAME"
for sol in sol-List sol-ListOpt sol-Explicit sol-ExplicitOpt; do
    echo -e "\t$sol: "
    for input_filename in $(find "$INSTANCES_DIRECTORY" -type f); do
        echo -e "\t\t$input_filename"
        \time -f "%U" "./solutions/$sol.exe" < "$input_filename" > "/tmp/isometric-out" 2> "/tmp/isometric-time"
        NUM_VERTICES="$(head -n 1 "$input_filename")"
        TIME=$(cat /tmp/isometric-time)
        DIM=$(cat /tmp/isometric-out)
        echo "$sol $input_filename $TIME $NUM_VERTICES $DIM" >> "$RAW_RESULTS_FILENAME"
    done
    #run_tests "$GENERATOR" "$PARAMETERS" "./$sol"
done
