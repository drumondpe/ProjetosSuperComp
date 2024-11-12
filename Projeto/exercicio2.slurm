#!/bin/bash
#SBATCH --job-name=exercicio2
#SBATCH --output=exercicio2_output.txt
#SBATCH --error=exercicio2_error.txt
#SBATCH --ntasks=1               # Número de tarefas (executáveis simultâneos)
#SBATCH --cpus-per-task=4        # Número de threads OpenMP para esta tarefa
#SBATCH --time=01:00:00          # Tempo limite de execução
#SBATCH --mem=2048M               # Memória alocada para a tarefa
#SBATCH --partition=espec        # Partição (ajuste conforme necessário)

# Mova para o diretório onde o job foi submetido
cd $SLURM_SUBMIT_DIR

# Executa o programa usando o arquivo de entrada especificado
./exercicio2 grafo35.txt
