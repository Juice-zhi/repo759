#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -c 8
#SBATCH -J Task4
#SBATCH -o Task4.out
#SBATCH -e Task4.err
#SBATCH -t 0-00:15:00

set -euo pipefail
g++ task3.cpp -Wall -O3 -std=c++17 -o task3 -fopenmp

# N-body simulation with 100 particles until time 100, for each OpenMP schedule
# and t = 1, ..., 8 threads. Each line of task4_timings.txt is
# "schedule t time_ms"; plot_task4.py turns it into task4.pdf.
rm -f task4_timings.txt
for schedule in static dynamic guided; do
    for ((t = 1; t <= 8; t++)); do
        result=$(./task3 100 100 "$t" "$schedule")
        echo "$schedule, t = $t: $result"
        echo "$schedule $t $(sed -n 's/^time: \(.*\)ms$/\1/p' <<< "$result")" >> task4_timings.txt
    done
done

# CPU model and core/thread layout of this node, for discussing the plots.
{
    lscpu
    grep Cpus_allowed_list /proc/self/status
} > node_info.txt
