#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -J Task3
#SBATCH -o Task3.out
#SBATCH -e Task3.err
#SBATCH -t 0-00:20:00
#SBATCH --gpus-per-task=1
#SBATCH --mem=16G

module load nvidia/cuda
set -euo pipefail
nvcc task3.cu vscale.cu -Xcompiler -O3 -Xcompiler -Wall -Xptxas -O3 -std=c++17 -o task3

# vscale for n = 2^10, ..., 2^29 with 512 and with 16 threads per block
# (n = 2^29 needs two 2 GiB arrays on the host and on the GPU). Each line of
# task3_timings.txt is "threads_per_block n time_ms"; plot_task3.py turns it
# into task3.pdf.
rm -f task3_timings.txt
for threads in 512 16; do
    for ((p = 10; p <= 29; p++)); do
        n=$((1 << p))
        result=$(./task3 "$n" "$threads")
        printf 'threads per block = %d, n = 2^%d\n%s\n' "$threads" "$p" "$result"
        echo "$threads $n $(head -n 1 <<< "$result")" >> task3_timings.txt
    done
done

# GPU model and memory, for discussing the plot.
nvidia-smi --query-gpu=name,memory.total,clocks.max.memory --format=csv > gpu_info.txt
