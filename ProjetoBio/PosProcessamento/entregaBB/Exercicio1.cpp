// Exercise1.cpp
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // for remove_if
#include <cctype>    // for isspace, toupper

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Defina o intervalo de cromossomos a serem processados
    int start_chromosome = 1; // Cromossomo inicial (altere para o valor desejado)
    int end_chromosome = 22;   // Cromossomo final (altere para o valor desejado)

    // Inicializa os contadores locais e globais
    long long local_counts[4] = {0, 0, 0, 0}; // A, C, G, T
    long long global_counts[4] = {0, 0, 0, 0};

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
        // Define o caminho completo do arquivo na pasta PosProcessamento
        std::string filename = "/home/pedrogdsd/SCRATCH/ProjetosSuperComp/ProjetoBio/PosProcessamento/chr" + std::to_string(chr) + "_processed.subst.fa";
        std::ifstream infile(filename);
        if (!infile) {
            std::cerr << "Erro ao abrir o arquivo " << filename << std::endl;
            continue;
        }

        std::string line;
        // Lê o arquivo e processa as sequências
        while (std::getline(infile, line)) {
            // Remove espaços em branco da linha
            line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

            if (line.empty()) continue;

            if (line[0] == '>') {
                // Linha de cabeçalho, pula
                continue;
            } else {
                // Converte para maiúsculas e conta as bases
                #pragma omp parallel for reduction(+:local_counts[:4])
                for (size_t i = 0; i < line.size(); ++i) {
                    char c = std::toupper(line[i]);
                    switch (c) {
                        case 'A':
                            local_counts[0]++;
                            break;
                        case 'C':
                            local_counts[1]++;
                            break;
                        case 'G':
                            local_counts[2]++;
                            break;
                        case 'T':
                            local_counts[3]++;
                            break;
                        default:
                            break; // Ignora outros caracteres
                    }
                }
            }
        }
        infile.close();
    }

    // Reduz as contagens entre todos os processos MPI
    MPI_Reduce(local_counts, global_counts, 4, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Contagem total de bases:" << std::endl;
        std::cout << "A: " << global_counts[0] << std::endl;
        std::cout << "C: " << global_counts[1] << std::endl;
        std::cout << "G: " << global_counts[2] << std::endl;
        std::cout << "T: " << global_counts[3] << std::endl;
    }

    MPI_Finalize();
    return 0;
}
