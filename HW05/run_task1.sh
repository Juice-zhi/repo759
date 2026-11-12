#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -J Task1
#SBATCH -o Task1.out
#SBATCH -e Task1.err
#SBATCH -t 0-00:05:00
#SBATCH --gpus-per-task=1

module load nvidia/cuda
set -euo pipefail
nvcc task1.cu -Xcompiler -O3 -Xcompiler -Wall -Xptxas -O3 -std=c++17 -o task1
./task1
