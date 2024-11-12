#!/bin/bash
#SBATCH --job-name=exercicio2
#SBATCH --output=exercicio2_output.txt
#SBATCH --error=exercicio2_error.txt
#SBATCH --ntasks=4              # Número de processos MPI
#SBATCH --cpus-per-task=4       # Número de threads OpenMP por processo
#SBATCH --time=00:15:00
#SBATCH --mem=2048M                # Alocação de memória (ajuste conforme necessário)
#SBATCH --partition=espec

# Mova para o diretório onde o job foi submetido
cd $SLURM_SUBMIT_DIR

# Executa o programa
mpirun -np 4 ./exercicio2
