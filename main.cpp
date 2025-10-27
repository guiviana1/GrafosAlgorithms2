#include <fstream>
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <stack>     
#include <vector>    
#include <cctype>    
#include <iomanip>   
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>


struct V { std::string name, label; };
struct E { std::string label; };

using DG = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, V, E>;
using UG = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, V, E>;

bool is_digraph(std::istream& is) {
    std::streampos p = is.tellg();
    std::string first_line;
    std::getline(is, first_line);
    is.clear();              
    is.seekg(p);              
    for (auto& c : first_line) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return first_line.find("digraph") != std::string::npos;
}

std::vector<int> bfs_levels_from_matrix(const std::vector<std::vector<int>>& matriz, int s) {
            int n = (int)matriz.size();
            const int INF = INT_MAX;
            std::vector<int> dist(n, INF);
            /*std::queue<int> q;
            dist[s] = 0;
            q.push(s);
            while (!q.empty()) {
                int u = q.front(); q.pop();
                for (int v = 0; v < n; ++v) {
                    if (matriz[u][v] != 0 && dist[v] == INF) {
                        dist[v] = dist[u] + 1;
                        q.push(v);
                    }
                }
            }*/
            return dist; 
}
