#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -c 1
#SBATCH -J Task6
#SBATCH -o Task6.out
#SBATCH -e Task6.err
#SBATCH -t 0-00:01:00

set -e
g++ task6.cpp -Wall -O3 -std=c++17 -o task6
./task6 6
