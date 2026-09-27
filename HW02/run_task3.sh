#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -c 1
#SBATCH -J Task3
#SBATCH -o Task3.out
#SBATCH -e Task3.err
#SBATCH -t 0-00:10:00

set -euo pipefail
g++ task3.cpp matmul.cpp -Wall -O3 -std=c++17 -o task3
./task3
