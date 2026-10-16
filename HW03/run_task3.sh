#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -c 20
#SBATCH -J Task3
#SBATCH -o Task3.out
#SBATCH -e Task3.err
#SBATCH -t 0-00:10:00

set -euo pipefail
g++ task3.cpp msort.cpp -Wall -O3 -std=c++17 -o task3 -fopenmp

n=1000000
rm -f task3_ts_timings.txt task3_t_timings.txt

# Part 1: t = 8 threads, threshold ts = 2^1, ..., 2^10.
# Each line of task3_ts_timings.txt is "ts time_ms" (-> task3_ts.pdf).
for ((p = 1; p <= 10; p++)); do
    ts=$((1 << p))
    result=$(./task3 "$n" 8 "$ts")
    printf 't = 8, ts = %d\n%s\n' "$ts" "$result"
    echo "$ts $(tail -n 1 <<< "$result")" >> task3_ts_timings.txt
done

# Part 2: t = 1, ..., 20 threads with the fastest ts from part 1.
# Each line of task3_t_timings.txt is "t time_ms" (-> task3_t.pdf).
best_ts=$(awk 'NR == 1 || $2 + 0 < best { best = $2 + 0; ts = $1 } END { print ts }' task3_ts_timings.txt)
echo "fastest ts = $best_ts"
for ((t = 1; t <= 20; t++)); do
    result=$(./task3 "$n" "$t" "$best_ts")
    printf 't = %d, ts = %d\n%s\n' "$t" "$best_ts" "$result"
    echo "$t $(tail -n 1 <<< "$result")" >> task3_t_timings.txt
done
