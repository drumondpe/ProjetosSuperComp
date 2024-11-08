// Exercicio4.cpp
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm> // para remove_if
#include <cctype>    // para isspace, toupper

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Defina o intervalo de cromossomos a serem processados
    int start_chromosome = 1; // Cromossomo inicial
    int end_chromosome = 22;  // Cromossomo final

    // Mapeamento de códons para aminoácidos
    std::map<std::string, std::string> codon_map = {
        {"CCA", "Prolina"}, {"CCG", "Prolina"}, {"CCU", "Prolina"}, {"CCC", "Prolina"},
        {"UCU", "Serina"}, {"UCA", "Serina"}, {"UCG", "Serina"}, {"UCC", "Serina"},
        {"CAG", "Glutamina"}, {"CAA", "Glutamina"},
        {"ACA", "Treonina"}, {"ACC", "Treonina"}, {"ACU", "Treonina"}, {"ACG", "Treonina"},
        {"AUG", "Metionina"}, // Códon de início
        {"UGA", "STOP"},      // Códon STOP
        {"UGC", "Cisteína"}, {"UGU", "Cisteína"},
        {"GUG", "Valina"}, {"GUA", "Valina"}, {"GUC", "Valina"}, {"GUU", "Valina"}
    };

    // Inicializa a contagem local de aminoácidos
    std::map<std::string, long long> local_amino_acid_counts;
    for (const auto& pair : codon_map) {
        if (pair.second != "STOP") {
            local_amino_acid_counts[pair.second] = 0;
        }
    }

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
        std::string filename = "/home/pedrogdsd/SCRATCH/ProjetosSuperComp/ProjetoBio/PosProcessamento/chr" + std::to_string(chr) + "_processed.rna.fa";
        std::ifstream infile(filename);
        if (!infile) {
            std::cerr << "Erro ao abrir o arquivo " << filename << std::endl;
            continue;
        }

        std::string line;
        std::vector<std::string> sequences;

        // Lê o arquivo e armazena as sequências
        while (std::getline(infile, line)) {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            if (line.empty()) continue;
            if (line[0] == '>') continue;

            for (char &c : line) {
                c = std::toupper(c);
            }
            sequences.push_back(line);
        }
        infile.close();

        // Contagem de aminoácidos nas sequências
        #pragma omp parallel for
        for (size_t seq_idx = 0; seq_idx < sequences.size(); ++seq_idx) {
            std::string &seq = sequences[seq_idx];
            bool start_found = false;
            std::map<std::string, long long> thread_amino_acid_counts;

            for (size_t i = 0; i + 2 < seq.size(); ++i) {
                std::string codon = seq.substr(i, 3);
                if (!start_found) {
                    if (codon == "AUG") {
                        start_found = true;
                        i += 2; // Avança para o próximo códon após o 'AUG'
                    } else {
                        continue;
                    }
                }

                if (start_found) {
                    if (codon == "UGA") break; // Para ao encontrar o códon STOP
                    if (codon_map.find(codon) != codon_map.end()) {
                        std::string amino_acid = codon_map[codon];
                        #pragma omp atomic
                        local_amino_acid_counts[amino_acid]++;
                    }
                }
            }
        }

        std::cout << "Processo " << rank << " concluiu a contagem no cromossomo " << chr << std::endl;
    }

    // Reduz as contagens locais para obter o total global
    std::map<std::string, long long> global_amino_acid_counts;
    for (const auto& pair : local_amino_acid_counts) {
        long long global_count;
        MPI_Reduce(&local_amino_acid_counts[pair.first], &global_count, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        global_amino_acid_counts[pair.first] = global_count;
    }

    if (rank == 0) {
        std::cout << "Contagem total de aminoácidos antes do códon STOP:" << std::endl;
        for (const auto& pair : global_amino_acid_counts) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
