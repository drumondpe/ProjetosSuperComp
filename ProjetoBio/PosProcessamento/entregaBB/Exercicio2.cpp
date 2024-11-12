// Exercicio2.cpp
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Defina o intervalo de cromossomos a serem processados
    int start_chromosome = 1; // Cromossomo inicial (altere para o valor desejado)
    int end_chromosome = 22;   // Cromossomo final (altere para o valor desejado)

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
        // Define os caminhos completos dos arquivos de entrada e saída
        std::string input_filename = "/home/pedrogdsd/SCRATCH/ProjetosSuperComp/ProjetoBio/PosProcessamento/chr" + std::to_string(chr) + "_processed.subst.fa";
        std::string output_filename = "/home/pedrogdsd/SCRATCH/ProjetosSuperComp/ProjetoBio/PosProcessamento/chr" + std::to_string(chr) + "_processed.rna.fa";

        std::ifstream infile(input_filename);
        if (!infile) {
            std::cerr << "Erro ao abrir o arquivo de entrada " << input_filename << std::endl;
            continue;
        }

        std::ofstream outfile(output_filename);
        if (!outfile) {
            std::cerr << "Erro ao abrir o arquivo de saída " << output_filename << std::endl;
            continue;
        }

        std::string line;

        // Lê o arquivo de entrada e converte a sequência de DNA para RNA
        while (std::getline(infile, line)) {
            if (line.empty()) continue;

            if (line[0] == '>') {
                // Linha de cabeçalho, escreve diretamente no arquivo de saída
                outfile << line << std::endl;
            } else {
                // Converte cada linha de DNA para RNA
                #pragma omp parallel for
                for (size_t i = 0; i < line.size(); ++i) {
                    if (line[i] == 'T') {
                        line[i] = 'U';
                    }
                }
                outfile << line << std::endl;
            }
        }

        infile.close();
        outfile.close();

        std::cout << "Processo " << rank << " concluiu a conversão do cromossomo " << chr << std::endl;
    }

    MPI_Finalize();
    return 0;
}
