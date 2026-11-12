#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -J Task2
#SBATCH -o Task2.out
#SBATCH -e Task2.err
#SBATCH -t 0-00:05:00
#SBATCH --ntasks=1
#SBATCH --gpus-per-task=1

module load nvidia/cuda
set -euo pipefail
nvcc task2.cu -Xcompiler -O3 -Xcompiler -Wall -Xptxas -O3 -std=c++17 -o task2
./task2
