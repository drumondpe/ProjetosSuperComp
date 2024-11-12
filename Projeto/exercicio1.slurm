#!/bin/bash
#SBATCH --job-name=exercicio1              # Nome do job
#SBATCH --output=exercicio1_output.txt     # Nome do arquivo de saída
#SBATCH --error=exercicio1_error.txt       # Nome do arquivo de erro
#SBATCH --ntasks=1                         # Apenas um processo, pois é uma execução sequencial
#SBATCH --cpus-per-task=1                  # Apenas uma CPU
#SBATCH --time=02:00:00                    # Tempo máximo de execução
#SBATCH --mem=2048M                        # Alocação de memória (ajuste conforme necessário)
#SBATCH --partition=espec                  # Partição a ser utilizada

# Mova para o diretório onde o job foi submetido
cd $SLURM_SUBMIT_DIR

# Executa o programa sequencialmente
./busca_exaustiva grafo35.txt
