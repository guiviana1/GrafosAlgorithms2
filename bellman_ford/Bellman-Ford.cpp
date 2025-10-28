#include <fstream>
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <utility>
#include <queue>
#include <limits>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

struct V { std::string name, label; };
struct E { std::string label; };

using DG = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, V, E>;
using UG = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, V, E>;

std::map<std::string, int> Bellman_Ford (
    std::string start, 
    std::set<std::string> pontos, 
    std::map<std::string, std::set<std::string>> listaadj, 
    std::map<std::pair<std::string, std::string>, int> aresta_peso, 
    bool& ciclo_negativo 
    ) {
    std::map<std::string, int> distancia;
    distancia[start] = 0; //distância do ponto inicial e zero

    // Definindo uma distância para todos os pontos
    for (auto it : pontos) {
        if (it != start){
            distancia[it] = std::numeric_limits<int>::max(); //definindo uma distancia máxima para modificar os resultados
        }
    }

    std::vector<std::string> ordem; //vetor para corrigir a posicao do start
    ordem.push_back(start);
    for (auto it : pontos){
        if (it != start){
            ordem.push_back(it);
        }
    }

    bool trocas = true;
    int iteracoes = 0;
    int maximo_iteraçoes = ordem.size() - 1; // Até |V|-1 iterações

    while (trocas && iteracoes < maximo_iteraçoes) {
        trocas = false;
        for (auto it : ordem) {
            for (auto i : listaadj[it]) {
                if (distancia[it] != std::numeric_limits<int>::max() &&
                    aresta_peso[{it, i}] + distancia[it] < distancia[i]) {
                    trocas = true;
                    distancia[i] = aresta_peso[{it, i}] + distancia[it];
                }
            }
        }
        iteracoes++;
    }

    // Verificação extra para detectar ciclos negativos
    ciclo_negativo = false;
    for (auto it : ordem) {
        for (auto i : listaadj[it]) {
            if (distancia[it] != std::numeric_limits<int>::max() &&
                aresta_peso[{it, i}] + distancia[it] < distancia[i]) {
                ciclo_negativo = true;
                break;
            }
        }
        if (ciclo_negativo) break;
    }

    return distancia;
}

void exibir_distancias(const std::map<std::string, int>& distancia, std::string start) {
    std::cout << "Menores custos partindo de " << start << ": \n";
    for (const auto& par : distancia) {
        std::cout << "Vértice: " << par.first << " - Custo: ";
        if (par.second == std::numeric_limits<int>::max())
            std::cout << "∞";  
        else
            std::cout << par.second;
        std::cout << "\n";
    }
}


bool is_digraph(std::istream& is) {
    std::streampos p = is.tellg();
    std::string first_line;
    std::getline(is, first_line);
    is.clear();              
    is.seekg(p);              
    for (auto& c : first_line) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return first_line.find("digraph") != std::string::npos;
}

int main() {
    std::string caminho;
    std::cout << "Insira o nome do arquivo a ser lido: ";
    std::cin >> caminho;

    std::ifstream in(caminho);
    if (!in) { std::cerr << caminho << " não existe\n"; return 1; }

    std::string start;

    bool directed = is_digraph(in);

    in.clear();
    in.seekg(0);

    if (directed) {
        DG g;
        boost::dynamic_properties dp(boost::ignore_other_properties);
        dp.property("node_id", get(&V::name, g));
        dp.property("label",   get(&V::label, g));
        dp.property("label",   get(&E::label, g));
        if (!boost::read_graphviz(in, g, dp)) { std::cerr << "falha DOT\n"; return 2; }
        std::cout << "Grafo dirigido lido.\n";
        // for (auto e : boost::make_iterator_range(edges(g)))
        //     std::cout << g[source(e,g)].name << " -> " << g[target(e,g)].name
        //               << " label=" << g[e].label << "\n";

        std::map<std::string, std::set<std::string>> listaadj;
        for (auto e : boost::make_iterator_range(edges(g))) {
            const std::string& u = g[source(e, g)].name;
            const std::string& v = g[target(e, g)].name;
            listaadj[u].insert(v); 
        }
        std::set<std::string> pontos; //set com todos os pontos do grafo
        for (auto e : boost::make_iterator_range(edges(g))) {
            const std::string& u = g[source(e, g)].name;
            const std::string& v = g[target(e, g)].name;
            pontos.insert(v); 
            pontos.insert(u);
        }

        while (true) {
            std::cout << "Insira o vértice inicial: ";
            std::cin >> start;
            if (pontos.find(start) != pontos.end()) {
                break; // vértice válido encontrado, sai do loop
            } else {
                std::cout << "Esse vértice não existe no grafo. Tente novamente.\n";
            }
        }

        std::map<std::pair<std::string, std::string>, int> aresta_peso; //map que guarda os pares e pesos
        for (auto e : boost::make_iterator_range(edges(g))) {
            auto u = source(e, g);
            auto v = target(e, g);
            int peso = 0;
            try {
                peso = std::stoi(g[e].label); // converte label string para int
            } catch (...) {
                peso = 1; // valor padrão se conversão falhar
            }
            aresta_peso[{g[u].name, g[v].name}] = peso; // armazena peso no mapa
        }

        std::map<std::string, int> distancia; //map para armazenar a distancia ate certo ponto
        bool ciclo_negativo; // booleana que armazena se temos ou nao um ciclo negativo
        distancia = Bellman_Ford(start, pontos, listaadj, aresta_peso, ciclo_negativo);
        exibir_distancias(distancia, start);
       
        // caso tenhamos um ciclo negativo
        if (ciclo_negativo) 
        std::cout << "ATENÇÃO❗: ciclo negativo detectado no grafo. Distâncias podem não ser confiáveis.\n";

        return 0;
    }
}