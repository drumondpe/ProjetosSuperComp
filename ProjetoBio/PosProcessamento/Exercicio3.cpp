// Exercicio3.cpp
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // para remove_if
#include <cctype>    // para isspace, toupper

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Defina o intervalo de cromossomos a serem processados
    int start_chromosome = 1; // Cromossomo inicial (altere para o valor desejado)
    int end_chromosome = 22;   // Cromossomo final (altere para o valor desejado)

    // Inicializa o contador local e global
    long long local_count = 0;
    long long global_count = 0;

    // Gera a lista de cromossomos a serem processados
    std::vector<int> chromosomes;
    for (int i = start_chromosome; i <= end_chromosome; ++i) {
        chromosomes.push_back(i);
    }

    // Distribui os cromossomos entre os processos MPI
    std::vector<int> my_chromosomes;
    for (size_t i = 0; i < chromosomes.size(); ++i) {
        if (i % size == rank) {
            my_chromosomes.push_back(chromosomes[i]);
        }
    }

    // Processa os cromossomos atribuídos
    for (int chr : my_chromosomes) {
        // Define o caminho completo do arquivo RNA gerado no exercício 2
        std::string filename = "/home/pedrogdsd/SCRATCH/ProjetosSuperComp/ProjetoBio/PosProcessamento/chr" + std::to_string(chr) + "_processed.rna.fa";
        std::ifstream infile(filename);
        if (!infile) {
            std::cerr << "Erro ao abrir o arquivo " << filename << std::endl;
            continue;
        }

        std::string line;
        // Lê o arquivo e processa as sequências
        while (std::getline(infile, line)) {
            // Remove espaços em branco da linha
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

            if (line.empty()) continue;

            if (line[0] == '>') {
                // Linha de cabeçalho, pula
                continue;
            } else {
                // Converte a linha para maiúsculas (se necessário)
                for (char &c : line) {
                    c = std::toupper(c);
                }

                // Busca por ocorrências do códon 'AUG'
                #pragma omp parallel
                {
                    long long thread_count = 0;
                    #pragma omp for nowait
                    for (long i = 0; i <= (long)line.size() - 3; i++) {
                        if (line[i] == 'A' && line[i+1] == 'U' && line[i+2] == 'G') {
                            thread_count++;
                        }
                    }
                    #pragma omp atomic
                    local_count += thread_count;
                }
            }
        }
        infile.close();

        std::cout << "Processo " << rank << " concluiu a contagem no cromossomo " << chr << std::endl;
    }

    // Reduz as contagens entre todos os processos MPI
    MPI_Reduce(&local_count, &global_count, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Número total de proteínas inicializadas (códons 'AUG'): " << global_count << std::endl;
    }

    MPI_Finalize();
    return 0;
}
