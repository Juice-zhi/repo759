#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -c 20
#SBATCH -J Task2
#SBATCH -o Task2.out
#SBATCH -e Task2.err
#SBATCH -t 0-00:05:00

set -euo pipefail
g++ task2.cpp convolution.cpp -Wall -O3 -std=c++17 -o task2 -fopenmp

# convolve with n = 1024 for t = 1, ..., 20 threads. Each line of
# task2_timings.txt is "t time_ms"; plot_hw03.py turns it into task2.pdf.
rm -f task2_timings.txt
for ((t = 1; t <= 20; t++)); do
    result=$(./task2 1024 "$t")
    printf 't = %d\n%s\n' "$t" "$result"
    echo "$t $(tail -n 1 <<< "$result")" >> task2_timings.txt
done

# CPU model and core/thread layout of this node, for discussing the plot.
{
    lscpu
    grep Cpus_allowed_list /proc/self/status
} > node_info.txt
