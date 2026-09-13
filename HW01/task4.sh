#!/usr/bin/env bash
#SBATCH -p instruction
#SBATCH -c 2
#SBATCH -J FirstSlurm
#SBATCH -o FirstSlurm.out
#SBATCH -e FirstSlurm.err
#SBATCH -t 0-00:01:00

hostname
