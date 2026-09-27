#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -c 1
#SBATCH -J Task2
#SBATCH -o Task2.out
#SBATCH -e Task2.err
#SBATCH -t 0-00:05:00

set -euo pipefail
g++ convolution.cpp task2.cpp -Wall -O3 -std=c++17 -o task2

for args in "4 3" "1024 3" "1024 7"; do
    read -r n m <<< "$args"
    echo "./task2 $n $m"
    ./task2 "$n" "$m"
done
