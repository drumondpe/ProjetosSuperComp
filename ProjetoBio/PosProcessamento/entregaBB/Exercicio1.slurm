#!/bin/bash
#SBATCH --job-name=exercicio1
#SBATCH --output=exercicio1_output.txt
#SBATCH --error=exercicio1_error.txt
#SBATCH --ntasks=4              # Number of MPI processes
#SBATCH --cpus-per-task=4       # Number of OpenMP threads per process
#SBATCH --time=00:10:00
#SBATCH --mem=2048M             # Memory allocation (adjust as needed)
#SBATCH --partition=espec

# Move to the directory where the job was submitted
cd $SLURM_SUBMIT_DIR

# Print the current working directory and list files (for debugging)
pwd
ls -l PosProcessamento

# Run the program
mpirun -np 4 ./exercicio1
