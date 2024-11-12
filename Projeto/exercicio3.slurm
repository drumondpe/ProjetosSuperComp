#!/bin/bash
#SBATCH --job-name=exercicio3
#SBATCH --output=exercicio3_output.txt
#SBATCH --error=exercicio3_error.txt
#SBATCH --ntasks=4               # Número de processos MPI
#SBATCH --cpus-per-task=1        # Cada processo MPI usará 1 CPU
#SBATCH --time=01:00:00          # Tempo limite de execução
#SBATCH --mem=2048M              # Memória alocada para a tarefa
#SBATCH --partition=espec        # Partição (ajuste conforme necessário)

# Mova para o diretório onde o job foi submetido
cd $SLURM_SUBMIT_DIR

# Executa o programa MPI usando o arquivo de entrada especificado
mpirun -np $SLURM_NTASKS ./exercicio3 grafo35.txt
