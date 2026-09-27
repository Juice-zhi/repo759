#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -c 1
#SBATCH -J Task1
#SBATCH -o Task1.out
#SBATCH -e Task1.err
#SBATCH -t 0-00:20:00
#SBATCH --mem=16G

set -euo pipefail
g++ scan.cpp task1.cpp -Wall -O3 -std=c++17 -o task1

# Scaling analysis for n = 2^10, ..., 2^30 (n = 2^30 needs two 4 GiB arrays).
# Each line of task1_timings.txt is "n time_ms"; plot_task1.py turns it into task1.pdf.
rm -f task1_timings.txt
for ((p = 10; p <= 30; p++)); do
    n=$((1 << p))
    result=$(./task1 "$n")
    printf 'n = 2^%d = %d\n%s\n' "$p" "$n" "$result"
    echo "$n $(head -n 1 <<< "$result")" >> task1_timings.txt
done
