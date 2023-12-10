#ifndef LIST_H
#define LIST_H

// definicao do tipo de funcao generica
typedef void (*FuncaoPonteiro)(void *);

// definicao da estrutura lista
typedef struct list
{

    struct list *prox;         // proxima coordenada
    void *item;                // posicao referencia List
    FuncaoPonteiro deallocate; // referencia da funcao para realizar a limpeza da variavel apontada

} List;

// definicao da estrutura coordenada
typedef struct listsorted
{

    struct listsorted *prox;   // proxima coordenada
    void *item;                // posicao referencia List
    FuncaoPonteiro deallocate; // referencia da funcao para realizar a limpeza da variavel apontada
    double fitness;            // fitness
    double frequency;          // frequencia

} ListSorted;

//------------------- LIST -----------------------

// funcao para criar uma nova celula ordenada ASC
List *ListInsert(List *list, void *data, FuncaoPonteiro func_deallocate);

// funcao para criar uma nova celula ao fim da List
List *ListInsertEnd(List *list, void *data, FuncaoPonteiro func_deallocate);

// remover todos os itens criados da List
void ListRemove(List *list);

// obter ultima posicao
void *ListGetLastPosition(List *list);

// obter posicao N
void *ListGetPosicao(List *list, int n);

// obter tamanho da lista
int ListSize(List *list);

// Remover posicao de uma lista
List *ListRemovePosition(List *list, int n);

//------------------- LIST SORTED -----------------------

// funcao para criar uma nova celula ordenada ASC
void ListSortedInsert(ListSorted **list, void *data, FuncaoPonteiro func_deallocate, double value);

// remover todos os itens criados da List
void ListSortedRemove(ListSorted *list);

// Remover posicao de uma lista
ListSorted *ListSortedRemovePosition(ListSorted *list, int n);

// obter tamanho da lista
int ListSortedSize(ListSorted *list);

// obter posicao
void *ListSortedGetPosition(ListSorted *list, int n);

// realizar um pop em uma posicao de uma lista, deve retornar o item
void *ListSortedPopPosition(ListSorted **list, int n);

// update fitness
void ListSortedUpdateFitness(ListSorted *list, int n, double fitness);

// ordenar lista
void ListSortedMergeSort(ListSorted **list);
void ListSortedMergeSplit(ListSorted *source, ListSorted **left, ListSorted **right);
ListSorted *ListSortedMerge(ListSorted *left, ListSorted *right);


#endif