#!/bin/bash

# Diretórios de entrada e saída
input_dir="PreProcessamento"
output_dir="PosProcessamento"

# Cria o diretório de saída, se ele não existir
mkdir -p "$output_dir"

# Loop para processar todos os arquivos dos cromossomos 1 a 22
for i in {1..22}
do
  input_file="${input_dir}/chr${i}.subst.fa"
  output_file="${output_dir}/chr${i}_processed.subst.fa"
  
  # Verifica se o arquivo de entrada existe antes de processar
  if [[ -f "$input_file" ]]; then
    # Converte para maiúsculas primeiro e depois remove todos os caracteres que não sejam A, C, G ou T
    tr '[:lower:]' '[:upper:]' < "$input_file" | sed '/^>/! s/[^ACGT]//g' > "$output_file"
    echo "Arquivo chr${i}.subst.fa processado e salvo em PosProcessamento."
  else
    echo "Arquivo chr${i}.subst.fa não encontrado em PreProcessamento."
  fi
done

echo "Processamento concluído para todos os cromossomos!"
