#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -c 1
#SBATCH -J Task2
#SBATCH -o Task2.out
#SBATCH -e Task2.err
#SBATCH -t 0-00:05:00

set -euo pipefail
g++ task2.cpp -Wall -O3 -std=c++17 -o task2

# Serial N-body simulation: 100 particles until time 100.
./task2 100 100
