#ifndef NODE_H
#define NODE_H

// estrutura com as variaveis de cada nó do grafo
typedef struct node
{
    int id;
    int demand;
    double x;
    double y;
} Node;

// criar novo node
Node *NodeNew(int id, double x, double y);

//Recebe uma string que contem os parâmetros de uma estrutura Node
void NodeSetValues(Node *node, char *str);

// Printar node
void NodePrint(Node *node);

#endif