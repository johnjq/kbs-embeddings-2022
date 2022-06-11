#!/usr/bin/env bash
set -e

RAW_RESULTS_FILENAME="raw_results_for_synthetic_instances.txt"
NUM_SAMPLES=100

generate_tests() {
    GENERATOR="$1"
    PARAMETERS="$2"
    for i in $(seq 1 $NUM_SAMPLES); do
        "./generators/$GENERATOR.exe" "$i" <<< "$PARAMETERS" > "/tmp/isometric-in-$i"
    done
}

run_test() {
    SOLUTION="$1"
    TEST="$2"
    \time -f "%U" "./solutions/$SOLUTION.exe" < "/tmp/isometric-in-$i" > "/tmp/isometric-out" 2> "/tmp/isometric-time"
}

run_tests() {
    GENERATOR="$1"
    PARAMETERS="$2"
    SOLUTION="$3"
    SUM_TIME=0
    SUM_DIM=0
    NUM=0
    for i in $(seq 1 $NUM_SAMPLES); do
        run_test "$SOLUTION" "$i"
        TIME=$(cat /tmp/isometric-time)
        DIM=$(cat /tmp/isometric-out)
        SUM_TIME=$(bc <<< "scale=10; $SUM_TIME + $TIME")
        SUM_DIM=$(bc <<< "scale=10; $SUM_DIM + $DIM")
        NUM=$((NUM+1))
        echo "$GENERATOR $PARAMETERS $SOLUTION $TIME $DIM" >> "$RAW_RESULTS_FILENAME"
    done
    AVG_TIME=$(bc <<< "scale=10; $SUM_TIME / $NUM")
    AVG_DIM=$(bc <<< "scale=10; $SUM_DIM / $NUM")
    printf 'Avg runtime: %3.4f (avg dimensionality: %3.2f)\n' "$AVG_TIME" "$AVG_DIM"
}

compare() {
    GENERATOR="$1"
    PARAMETERS="$2"
    echo "$GENERATOR:"
    generate_tests "$GENERATOR" "$PARAMETERS"
    for sol in sol-List sol-ListOpt sol-Explicit sol-ExplicitOpt; do
        echo -ne "\t$sol: "
        run_tests "$GENERATOR" "$PARAMETERS" "$sol"
    done
}

echo "=> Compiling generators..."
g++ -std=c++11 -O2 generators/gen-random.cpp -o generators/gen-random.exe
g++ -std=c++11 -O2 generators/gen-star.cpp -o generators/gen-star.exe
g++ -std=c++11 -O2 generators/gen-banana.cpp -o generators/gen-banana.exe
g++ -std=c++11 -O2 generators/gen-bamboo.cpp -o generators/gen-bamboo.exe
g++ -std=c++11 -O2 generators/gen-caterpillar.cpp -o generators/gen-caterpillar.exe
g++ -std=c++11 -O2 generators/gen-binary.cpp -o generators/gen-binary.exe
g++ -std=c++11 -O2 generators/gen-ternary.cpp -o generators/gen-ternary.exe
g++ -std=c++11 -O2 generators/gen-quaternary.cpp -o generators/gen-quaternary.exe

echo "=> Compiling solutions..."
g++ -std=c++11 -O2 solutions/sol-List.cpp -o solutions/sol-List.exe
g++ -std=c++11 -O2 solutions/sol-ListOpt.cpp -o solutions/sol-ListOpt.exe
g++ -std=c++11 -O2 solutions/sol-Explicit.cpp -o solutions/sol-Explicit.exe
g++ -std=c++11 -O2 solutions/sol-ExplicitOpt.cpp -o solutions/sol-ExplicitOpt.exe

echo "=> Running..."
echo -n > "$RAW_RESULTS_FILENAME"
compare gen-random 100000
compare gen-star 100000
compare gen-banana 100000
compare gen-bamboo 100000
compare gen-caterpillar 100000
compare gen-binary 100000
compare gen-ternary 100000
compare gen-quaternary 100000
