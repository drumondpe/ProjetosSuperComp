// O que faz: Lê o grafo do arquivo grafo.txt e armazena como uma matriz de adjacência.
// Resultado: O grafo estará representado como uma matriz de adjacência.

#include <iostream>
#include <vector>
#include <fstream>

// Função para ler o grafo a partir do arquivo de entrada
std::vector<std::vector<int>> LerGrafo(const std::string& nomeArquivo, int& numVertices) {
    std::ifstream arquivo(nomeArquivo);
    int numArestas;
    arquivo >> numVertices >> numArestas;

    // Cria uma matriz de adjacência de tamanho numVertices x numVertices
    std::vector<std::vector<int>> grafo(numVertices, std::vector<int>(numVertices, 0));

    // Preenche a matriz com as arestas do grafo
    for (int i = 0; i < numArestas; ++i) {
        int u, v;
        arquivo >> u >> v;
        grafo[u - 1][v - 1] = 1;
        grafo[v - 1][u - 1] = 1;  // O grafo é não direcionado
    }

    arquivo.close();
    return grafo;
}
