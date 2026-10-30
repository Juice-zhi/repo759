#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -c 8
#SBATCH -J Task4
#SBATCH -o Task4.out
#SBATCH -e Task4.err
#SBATCH -t 0-00:30:00

set -euo pipefail
g++ task3.cpp -Wall -O3 -std=c++17 -o task3 -fopenmp

# Pin each OpenMP thread to a core, filling distinct cores first, so the
# scheduler cannot put two of our threads on the same CPU.
export OMP_PROC_BIND=close
export OMP_PLACES=cores

# N-body simulation with 200 particles until time 100, for each OpenMP schedule
# and t = 1, ..., 8 threads. Single runs on this shared node occasionally take
# 10-100x longer, so every configuration runs three times (the repetitions are
# interleaved) and plot_task4.py plots the fastest run. A run that takes more
# than 60 s is stopped and gets no entry. Each line of task4_timings.txt is
# "schedule t time_ms".
rm -f task4_timings.txt
for rep in 1 2 3; do
    for schedule in static dynamic guided; do
        for ((t = 1; t <= 8; t++)); do
            if result=$(timeout 60 ./task3 200 100 "$t" "$schedule"); then
                echo "run $rep, $schedule, t = $t: $result"
                echo "$schedule $t $(sed -n 's/^time: \(.*\)ms$/\1/p' <<< "$result")" >> task4_timings.txt
            else
                echo "run $rep, $schedule, t = $t: stopped after 60 s"
            fi
        done
    done
done

# CPU model, core/thread layout and load of this node, for discussing the plots.
{
    lscpu
    grep Cpus_allowed_list /proc/self/status
    uptime
} > node_info.txt
