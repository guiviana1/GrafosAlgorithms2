#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <limits>
#include <cctype>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

using UG = boost::adjacency_list<
    boost::vecS, boost::vecS, boost::undirectedS,
    boost::property<boost::vertex_name_t, std::string>,
    boost::property<boost::edge_weight_t, double>
>;

using DG = boost::adjacency_list<
    boost::vecS, boost::vecS, boost::directedS,
    boost::property<boost::vertex_name_t, std::string>,
    boost::property<boost::edge_weight_t, double>
>;

bool is_digraph(std::istream& is) {
    std::streampos p = is.tellg();
    std::string first_line;
    std::getline(is, first_line);
    is.clear(); is.seekg(p);
    for (auto& c : first_line) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return first_line.find("digraph") != std::string::npos;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(&std::cout);

    std::string caminho;
    std::cout << "Insira o nome do arquivo a ser lido: ";
    std::cin >> caminho;

    std::ifstream in(caminho);
    if (!in) { std::cerr << caminho << " não existe\n"; return 1; }

    bool directed = is_digraph(in);
    UG g;

    if (directed) {
        DG dg;
        boost::dynamic_properties dp(boost::ignore_other_properties);
        dp.property("node_id", get(boost::vertex_name, dg));
        dp.property("label",   get(boost::edge_weight, dg)); // label -> peso (double)
        if (!boost::read_graphviz(in, dg, dp)) { std::cerr << "falha ao ler DOT (digraph)\n"; return 2; }

        g = UG(num_vertices(dg));
        auto name_dg = get(boost::vertex_name, dg);
        auto name_ug = get(boost::vertex_name, g);
        for (std::size_t i = 0; i < num_vertices(dg); ++i) name_ug[i] = name_dg[i];

        auto wdg = get(boost::edge_weight, dg);
        auto wug = get(boost::edge_weight, g);
        for (auto e : boost::make_iterator_range(edges(dg))) {
            auto u = source(e, dg);
            auto v = target(e, dg);
            double w = wdg[e];
            auto added = add_edge(u, v, g).first;
            wug[added] = w;
        }
    } else {
        boost::dynamic_properties dp(boost::ignore_other_properties);
        dp.property("node_id", get(boost::vertex_name, g));
        dp.property("label",   get(boost::edge_weight, g));
        if (!boost::read_graphviz(in, g, dp)) { std::cerr << "falha ao ler DOT (graph)\n"; return 2; }
    }

    const std::size_t n = num_vertices(g);
    if (n == 0) { std::cerr << "Grafo vazio\n"; return 0; }

    auto name_map = get(boost::vertex_name, g);
    auto wmap     = get(boost::edge_weight, g);

    std::vector<std::string> names(n);
    std::map<std::string, std::size_t> idx;
    for (std::size_t i = 0; i < n; ++i) {
        names[i] = name_map[i].empty() ? std::to_string(i) : name_map[i];
        idx[names[i]] = i;
    }

    std::string start;
    while (true) {
        std::cout << "Insira o vértice raiz do MST: ";
        std::cin >> start;
        if (idx.count(start)) break;
        std::cout << "Vértice inexistente. Tente novamente.\n";
    }
    std::size_t r = idx[start];

    const double INF = std::numeric_limits<double>::infinity();
    std::vector<double> key(n, INF);
    std::vector<int>    parent(n, -1);
    std::vector<char>   inQ(n, 1);

    key[r] = 0.0;
    using Item = std::pair<double, std::size_t>;
    struct Cmp { bool operator()(const Item& a, const Item& b) const { return a.first > b.first; } };
    std::priority_queue<Item, std::vector<Item>, Cmp> pq;
    for (std::size_t v = 0; v < n; ++v) pq.push({key[v], v});

    while (!pq.empty()) {
        auto [ku, u] = pq.top(); pq.pop();
        if (!inQ[u] || ku != key[u]) continue;
        inQ[u] = 0;

        auto nbr = adjacent_vertices(u, g);
        for (auto it = nbr.first; it != nbr.second; ++it) {
            std::size_t v = static_cast<std::size_t>(*it);
            auto ep = edge(u, v, g);
            if (!ep.second) continue;
            double w = wmap[ep.first];

            if (inQ[v] && w < key[v]) {
                key[v] = w;
                parent[v] = static_cast<int>(u);
                pq.push({key[v], v});
            }
        }
    }

    double total = 0.0;
    std::cout << "\nArestas da MST a partir de " << start << ":\n";
    for (std::size_t v = 0; v < n; ++v) {
        if (parent[v] >= 0 && key[v] < INF) {
            std::cout << names[parent[v]] << " -- " << names[v] << "  (w=" << key[v] << ")\n";
            total += key[v];
        }
    }
    std::cout << "Custo total da MST = " << total << "\n";

    bool desconectado = false;
    for (std::size_t v = 0; v < n; ++v) if (v != r && parent[v] == -1) {
         desconectado = true;
        break; 
    }
    if (desconectado) std::cout << "Aviso: grafo desconectado; MST cobre apenas o componente de " << start << ".\n";

    return 0;
}
