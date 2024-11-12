#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib> // Para usar exit()
#include <chrono>  // Para medir o tempo de execução
#include <omp.h>   // Biblioteca OpenMP

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
        grafo[v - 1][u - 1] = 1;
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

// Função de entrada paralelizada com OpenMP
void BuscaExaustivaParalela(const std::vector<std::vector<int>>& grafo, int numVertices, std::vector<int>& melhorClique) {
    #pragma omp parallel
    {
        std::vector<int> melhorCliqueLocal;
        std::vector<int> cliqueAtual;

        #pragma omp for schedule(dynamic)
        for (int i = 0; i < numVertices; i++) {
            cliqueAtual.push_back(i);
            BuscaExaustivaClique(grafo, numVertices, melhorCliqueLocal, cliqueAtual, i + 1);
            cliqueAtual.pop_back();

            #pragma omp critical
            {
                if (melhorCliqueLocal.size() > melhorClique.size()) {
                    melhorClique = melhorCliqueLocal;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <nome do arquivo de entrada>\n";
        return 1;
    }

    int numVertices;
    std::string nomeArquivo = argv[1];

    auto inicio = std::chrono::high_resolution_clock::now();

    std::vector<std::vector<int>> grafo = LerGrafo(nomeArquivo, numVertices);

    std::vector<int> melhorClique;
    BuscaExaustivaParalela(grafo, numVertices, melhorClique);

    auto fim = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duracao = fim - inicio;
    
    std::cout << "Tempo de execução: " << duracao.count() << " segundos." << std::endl;

    std::cout << "Clique máxima encontrada: ";
    for (int v : melhorClique) {
        std::cout << (v + 1) << " ";
    }
    std::cout << std::endl;

    std::cout << "Tamanho da clique máxima: " << melhorClique.size() << std::endl;

    return 0;
}
