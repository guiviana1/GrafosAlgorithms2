Descrição:
------------
Este projeto implementa o algoritmo de Bellman-Ford para cálculo dos caminhos mínimos em grafos direcionados, incluindo detecção de ciclos negativos.

O código lê um grafo no formato DOT, constrói as estruturas necessárias e calcula as distâncias mínimas a partir de um vértice origem fornecido pelo usuário.

Compilação:
------------
Para compilar o código, certifique-se de ter a biblioteca Boost Graph instalada.

Use o comando:

g++ -std=c++17 -O0 -g -o bellman_ford Bellman-Ford.cpp -lboost_graph

Explicação das flags:
-std=c++17    -> usa padrão C++17  
-O0           -> desativa otimizações para facilitar a depuração  
-g            -> gera informações de debug  
-lboost_graph -> linka a biblioteca Boost Graph  

Execução:
------------
Após compilado, execute o programa:

./bellman_ford

O programa solicitará o nome do arquivo DOT para leitura do grafo, e o vértice origem para calcular os caminhos mínimos.

Saída:
---------
- Exibe as distâncias mínimas para todos os vértices a partir da origem.  
- Indica se foi detectado ciclo negativo, avisando que as distâncias podem não ser confiáveis.  

Observações:
--------------
- O grafo deve estar no formato DOT com pesos nas arestas indicados no atributo "label".  
- Ciclos negativos são detectados e reportados pelo programa.  
- Existem exemplos de arquivos .dot na pasta do projeto:  
  - `ex.dot` para grafo sem ciclos negativos  
  - `ciclo_negativo.dot` para grafo com ciclo negativo  

Esses arquivos servem para testar e validar o funcionamento do algoritmo Bellman-Ford neste projeto.
