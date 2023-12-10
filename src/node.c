#include <stdio.h>
#include <stdlib.h>
#include "node.h"

// criar novo node
Node *NodeNew(int id, double x, double y)
{
    Node *node = (void *)malloc(sizeof(Node));
    node->id = id;
    node->x = x;
    node->y = y;
    return node;
}

//Recebe uma string que contem os parâmetros de uma estrutura Node
void NodeSetValues(Node* node, char* str){
    sscanf(str, " %d %lf %lf", &(node->id), &(node->x), &(node->y));
}

// Printar node
void NodePrint(Node* node){
    printf("\n");
    printf("Node: %d | Demanda: %d | x,y:(%f,%f)", node->id, node->demand, node->x, node->y);
}
