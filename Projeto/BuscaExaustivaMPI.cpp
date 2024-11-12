#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib> // Para usar exit()
#include <chrono>  // Para medir o tempo de execução
#include <mpi.h>   // Biblioteca MPI

// Função para ler o grafo a partir do arquivo de entrada
std::vector<std::vector<int>> LerGrafo(const std::string& nomeArquivo, int& numVertices) {
    std::ifstream arquivo(nomeArquivo);
    int numArestas;
    arquivo >> numVertices >> numArestas;

    std::vector<std::vector<int>> grafo(numVertices, std::vector<int>(numVertices, 0));

    for (int i = 0; i < numArestas; ++i) {
        int u, v;
        arquivo >> u >> v;
        grafo[u - 1][v - 1] = 1;
        grafo[v - 1][u - 1] = 1;  // O grafo é não direcionado
    }

    arquivo.close();
    return grafo;
}

// Função para verificar se um conjunto de vértices forma uma clique
bool VerificaClique(const std::vector<std::vector<int>>& grafo, const std::vector<int>& clique) {
    for (size_t i = 0; i < clique.size(); i++) {
        for (size_t j = i + 1; j < clique.size(); j++) {
            if (grafo[clique[i]][clique[j]] == 0) {
                return false;
            }
        }
    }
    return true;
}

// Função de busca exaustiva para encontrar a maior clique
void BuscaExaustivaClique(const std::vector<std::vector<int>>& grafo, int numVertices,
                          std::vector<int>& melhorClique, std::vector<int>& cliqueAtual, int vertice) {
    // Verifica se a clique atual é maior que a melhor clique encontrada
    if (cliqueAtual.size() > melhorClique.size() && VerificaClique(grafo, cliqueAtual)) {
        melhorClique = cliqueAtual;
    }

    // Tenta adicionar novos vértices à clique atual
    for (int i = vertice; i < numVertices; i++) {
        // Adiciona o vértice atual à clique e chama a função recursivamente
        cliqueAtual.push_back(i);
        BuscaExaustivaClique(grafo, numVertices, melhorClique, cliqueAtual, i + 1);
        cliqueAtual.pop_back(); // Remove o vértice após a chamada recursiva
    }
}

int main(int argc, char* argv[]) {
    int rank, size;

    // Inicializa o ambiente MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtém o rank do processo atual
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtém o número total de processos

    if (argc < 2) {
        if (rank == 0) {
            std::cerr << "Uso: " << argv[0] << " <nome do arquivo de entrada>\n";
        }
        MPI_Finalize();
        return 1;
    }

    int numVertices;
    std::string nomeArquivo = argv[1];
    std::vector<std::vector<int>> grafo;

    auto inicio = std::chrono::high_resolution_clock::now();

    // Processo 0 lê o grafo e o transmite para os outros processos
    if (rank == 0) {
        grafo = LerGrafo(nomeArquivo, numVertices);
    }

    // Transmite o número de vértices para todos os processos
    MPI_Bcast(&numVertices, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Ajusta o tamanho do grafo em todos os processos
    if (rank != 0) {
        grafo.resize(numVertices, std::vector<int>(numVertices, 0));
    }

    // Transmite o grafo para todos os processos
    for (int i = 0; i < numVertices; ++i) {
        MPI_Bcast(grafo[i].data(), numVertices, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Cada processo trabalha em um subconjunto dos vértices
    std::vector<int> melhorCliqueLocal;
    std::vector<int> cliqueAtual;

    // Distribui os vértices iniciais entre os processos
    for (int i = rank; i < numVertices; i += size) {
        cliqueAtual.push_back(i);
        BuscaExaustivaClique(grafo, numVertices, melhorCliqueLocal, cliqueAtual, i + 1);
        cliqueAtual.pop_back();
    }

    // Cada processo envia o tamanho da sua melhor clique para o processo 0
    int tamanhoCliqueLocal = melhorCliqueLocal.size();
    std::vector<int> tamanhosCliques(size);

    MPI_Gather(&tamanhoCliqueLocal, 1, MPI_INT, tamanhosCliques.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Processo 0 coleta as melhores cliques de cada processo
    std::vector<int> melhorCliqueGlobal;

    if (rank == 0) {
        // Prepara para receber as cliques
        std::vector<int> recvCounts(size);
        std::vector<int> displs(size);

        for (int i = 0; i < size; ++i) {
            recvCounts[i] = tamanhosCliques[i];
        }

        int totalSize = 0;
        for (int i = 0; i < size; ++i) {
            displs[i] = totalSize;
            totalSize += recvCounts[i];
        }

        std::vector<int> todasCliques(totalSize);

        // Coleta as cliques de todos os processos
        MPI_Gatherv(melhorCliqueLocal.data(), tamanhoCliqueLocal, MPI_INT,
                    todasCliques.data(), recvCounts.data(), displs.data(), MPI_INT, 0, MPI_COMM_WORLD);

        // Encontra a maior clique entre as recebidas
        int maxCliqueSize = 0;
        int maxCliqueOffset = 0;

        for (int i = 0; i < size; ++i) {
            if (tamanhosCliques[i] > maxCliqueSize) {
                maxCliqueSize = tamanhosCliques[i];
                maxCliqueOffset = displs[i];
            }
        }

        // Extrai a melhor clique
        melhorCliqueGlobal.assign(todasCliques.begin() + maxCliqueOffset,
                                  todasCliques.begin() + maxCliqueOffset + maxCliqueSize);

    } else {
        // Outros processos enviam suas cliques
        MPI_Gatherv(melhorCliqueLocal.data(), tamanhoCliqueLocal, MPI_INT,
                    nullptr, nullptr, nullptr, MPI_INT, 0, MPI_COMM_WORLD);
    }

    auto fim = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duracao = fim - inicio;

    if (rank == 0) {
        std::cout << "Tempo de execução: " << duracao.count() << " segundos." << std::endl;

        std::cout << "Clique máxima encontrada: ";
        for (int v : melhorCliqueGlobal) {
            std::cout << (v + 1) << " "; // Ajuste para índices baseados em 1
        }
        std::cout << std::endl;

        std::cout << "Tamanho da clique máxima: " << melhorCliqueGlobal.size() << std::endl;
    }

    MPI_Finalize();
    return 0;
}
