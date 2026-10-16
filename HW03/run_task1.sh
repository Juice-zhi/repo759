#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -c 20
#SBATCH -J Task1
#SBATCH -o Task1.out
#SBATCH -e Task1.err
#SBATCH -t 0-00:10:00

set -euo pipefail
g++ task1.cpp matmul.cpp -Wall -O3 -std=c++17 -o task1 -fopenmp

# mmul with n = 1024 for t = 1, ..., 20 threads. Each line of task1_timings.txt
# is "t time_ms"; plot_hw03.py turns it into task1.pdf.
rm -f task1_timings.txt
for ((t = 1; t <= 20; t++)); do
    result=$(./task1 1024 "$t")
    printf 't = %d\n%s\n' "$t" "$result"
    echo "$t $(tail -n 1 <<< "$result")" >> task1_timings.txt
done
