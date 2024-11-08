#!/bin/bash

# Loop para baixar e descompactar os arquivos dos cromossomos 1 até 22
for i in {1..22}
do
  # Baixar o arquivo do cromossomo correspondente
  wget "ftp://hgdownload.cse.ucsc.edu/goldenPath/hg19/snp147Mask/chr${i}.subst.fa.gz"

  # Descompactar o arquivo baixado
  gunzip "chr${i}.subst.fa.gz"
done

echo "Download e descompactação concluídos!"
