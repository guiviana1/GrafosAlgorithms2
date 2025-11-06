#include <fstream>
#include <iostream>
#include <string>
#include <set>      
#include <map>      
#include <utility>  
#include <limits>   
#include <vector>   
#include <iomanip>  
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

struct V { std::string name, label; };
struct E { std::string label; };

using DG = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, V, E>;


std::map<std::string, std::map<std::string, int>> Floyd_Warshall (
    const std::set<std::string>& pontos, 
    const std::map<std::pair<std::string, std::string>, int>& aresta_peso,
    bool& ciclo_negativo
) {
    std::map<std::string, std::map<std::string, int>> dist;
    
    const int INF = std::numeric_limits<int>::max();

    for (const std::string& i : pontos) {
        for (const std::string& j : pontos) {
            if (i == j) {
                dist[i][j] = 0; 
            } else {
                auto it = aresta_peso.find({i, j});
                if (it != aresta_peso.end()) {
                    dist[i][j] = it->second; 
                } else {
                    dist[i][j] = INF;
                }
            }
        }
    }

    for (const std::string& k : pontos) {
        for (const std::string& i : pontos) {
            for (const std::string& j : pontos) {
                if (dist[i][k] != INF && dist[k][j] != INF) {
                    if (dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                    }
                }
            }
        }
    }
    //detecção de ciclo negativo
    ciclo_negativo = false;
    for (const std::string& i : pontos) {
        if (dist[i][i] < 0) {
            ciclo_negativo = true;
            break;
        }
    }

    return dist;
}

void exibir_distancias(
    const std::map<std::string, std::map<std::string, int>>& distancias,
    const std::set<std::string>& pontos
) {
    std::cout << "Matriz resultado de caminhos mínimos entre vértice:\n";
    
    std::vector<std::string> ordem_pontos(pontos.begin(), pontos.end());

    const int LARGURA_COLUNA = 6;

    std::cout << std::setw(LARGURA_COLUNA) << "De\\Para|";
    for (const std::string& j : ordem_pontos) {
        std::cout << std::setw(LARGURA_COLUNA) << j << "|";
    }
    std::cout << "\n";

    std::cout << std::setfill('-') << std::setw((LARGURA_COLUNA + 1) * (ordem_pontos.size() + 1)) << "" << std::setfill(' ') << "\n";

    for (const std::string& i : ordem_pontos) {
        std::cout << std::setw(LARGURA_COLUNA) << i << "|"; 
        for (const std::string& j : ordem_pontos) {
            int valor = distancias.at(i).at(j);
            
            std::cout << std::setw(LARGURA_COLUNA);
            if (valor == std::numeric_limits<int>::max()) {
                std::cout << "∞";
            } else {
                std::cout << valor;
            }
            std::cout << "|";
        }
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

        std::set<std::string> pontos; 
        for (auto v : boost::make_iterator_range(vertices(g))) {
            pontos.insert(g[v].name);
        }
        
        for (auto e : boost::make_iterator_range(edges(g))) {
            pontos.insert(g[source(e, g)].name);
            pontos.insert(g[target(e, g)].name);
        }

        std::map<std::pair<std::string, std::string>, int> aresta_peso; 
        for (auto e : boost::make_iterator_range(edges(g))) {
            auto u = source(e, g);
            auto v = target(e, g);
            int peso = 0;
            try {
                peso = std::stoi(g[e].label); 
            } catch (...) {
                peso = 1; 
            }
            aresta_peso[{g[u].name, g[v].name}] = peso; 
        }

        
        std::cout << "\nExecutando Algoritmo de Floyd-Warshall:\n\n";

        bool fw_ciclo_negativo = false;
        std::map<std::string, std::map<std::string, int>> distancias_fw;
        
        distancias_fw = Floyd_Warshall(pontos, aresta_peso, fw_ciclo_negativo);
        
        exibir_distancias(distancias_fw, pontos);
        
        if (fw_ciclo_negativo) 
            std::cout << "\nCiclo negativo detectado no grafo.\n";
        return 0;

    } else {
        std::cerr << "O algoritmo de Floyd-Warshall é tipicamente usado para grafos dirigidos.\n";
        std::cerr << "O arquivo fornecido não parece ser um 'digraph'.\n";
        return 3;
    }
}