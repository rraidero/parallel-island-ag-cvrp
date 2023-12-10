#include <stdio.h>
#include <stdlib.h>
#include "list.h"

// funcao para criar uma nova celula ao fim da List
// recebe um ponteiro para a lista atual, a referencia dos dados e sua funcao para desalocar memoria dinamica
List *ListInsertEnd(List *list, void *data, FuncaoPonteiro func_deallocate)
{
    if (list == NULL)
    {
        list = (List *)malloc(sizeof(List));
        list->item = data;
        list->prox = NULL;
        list->deallocate = *func_deallocate;
    }
    else
    {
        List *pos_aux = list;
        // enquanto nao cheguei ao fim da fila
        while (pos_aux->prox != NULL)
        {
            pos_aux = pos_aux->prox;
        }
        // aloca nova celula
        List *n = (List *)malloc(sizeof(List));
        // aponta para a proxima posicao
        pos_aux->prox = n;
        n->item = data;
        n->prox = NULL;
        n->deallocate = *func_deallocate;
    }
    return list;
}

// remover todos os itens criados da List
void ListRemove(List *list)
{
    // enquanto minha nao eh a ultima posicao
    while (list != NULL)
    {
        // armazena a variavel aux
        List *temp = list;
        // pula List pra proxima
        list = list->prox;
        // limpa conteudo da List
        temp->deallocate(temp->item);
        // limpa List
        free(temp);
    }
}

// obter ultima posicao
void *ListGetLastPosition(List *list)
{
    List *lst = list;
    while (lst->prox != NULL)
    {
        // pula List pra proxima
        lst = lst->prox;
    }
    return lst->item;
}

// obter posicao N
void *ListGetPosicao(List *list, int n)
{
    List *aux = list;
    while (n > 0)
    {
        aux = aux->prox;
        n--;
    }
    return aux->item;
}

// obter tamanho da lista
int ListSize(List *list)
{
    int tam = 0;
    while (list != NULL)
    {
        list = list->prox;
        tam++;
    }
    return tam;
}

//Remover posicao N da lista, tambem desaloca
List *ListRemovePosition(List *list, int n)
{
    List *last = list;
    List *current = list->prox;
    if (n == 0)
    {
        last->deallocate(last->item);
        free(last);
        return current;
    }

    for (int i = 1; i < n; i++)
    {
        last = current;
        current = current->prox;
    }
    last->prox = current->prox;
    current->deallocate(current->item);
    free(current);
    return list;
}

//Inserir na lista na posicao de acordo com o fitness
void ListSortedInsert(ListSorted **list, void *data, FuncaoPonteiro func_deallocate, double value)
{
    ListSorted *new = (ListSorted *)malloc(sizeof(ListSorted));
    new->item = data;
    new->deallocate = *func_deallocate;
    new->fitness = value;
    new->prox = *list;
    *list = new;
}

//remover lista
void ListSortedRemove(ListSorted *list)
{
    // quanto minha nao eh a ultima
    while (list != NULL)
    {
        // armazena a variavel aux
        ListSorted *temp = list;
        // pula List pra proxima
        list = list->prox;
        // limpa conteudo da List
        temp->deallocate(temp->item);
        // limpa List
        free(temp);
    }
}

//retornar tamanho da lista
int ListSortedSize(ListSorted *list)
{
    int tam = 0;
    while (list != NULL)
    {
        list = list->prox;
        tam++;
    }
    return tam;
}

//remover posicao da lista
ListSorted *ListSortedRemovePosition(ListSorted *list, int n)
{
    ListSorted *last = list;
    ListSorted *current = list->prox;
    if (n == 0)
    {
        last->deallocate(last->item);
        free(last);
        return current;
    }
    for (int i = 1; i < n; i++)
    {
        last = current;
        current = current->prox;
    }
    last->prox = current->prox;
    current->deallocate(current->item);
    free(current);
    return list;
}

//enviar uma atualizacao do fitness para a lista
void ListSortedUpdateFitness(ListSorted *list, int n, double value)
{
    ListSorted *aux = list;
    while (n > 0)
    {
        aux = aux->prox;
        n--;
    }
    aux->fitness = value;
}

//obter posicao
void *ListSortedGetPosition(ListSorted *list, int n)
{
    ListSorted *aux = list;
    while (n > 0)
    {
        aux = aux->prox;
        n--;
    }
    return aux->item;
}

//obter uma posicao 'pop'
void *ListSortedPopPosition(ListSorted **list, int n)
{
    ListSorted *last = *list;
    ListSorted *current = (*list)->prox;
    if (n == 0)
    {
        *list = current;
        void *item = last->item;
        free(last);
        return item;
    }
    for (int i = 1; i < n; i++)
    {
        last = current;
        current = current->prox;
    }
    last->prox = current->prox;
    void *item = current->item;
    free(current);
    return item;
}

// Função de ordenação Merge Sort
void ListSortedMergeSort(ListSorted **list)
{
    ListSorted *headRef = *list;
    ListSorted *left;
    ListSorted *right;
    if (headRef == NULL || headRef->prox == NULL)
    {
        return;
    }
    ListSortedMergeSplit(headRef, &left, &right);
    ListSortedMergeSort(&left);
    ListSortedMergeSort(&right);
    *list = ListSortedMerge(left, right);
}

// Função para dividir a lista em duas partes
void ListSortedMergeSplit(ListSorted *source, ListSorted **left, ListSorted **right)
{
    if (source == NULL || source->prox == NULL)
    {
        *left = source;
        *right = NULL;
        return;
    }
    ListSorted *slow = source;
    ListSorted *fast = source->prox;
    while (fast != NULL)
    {
        fast = fast->prox;
        if (fast != NULL)
        {
            slow = slow->prox;
            fast = fast->prox;
        }
    }
    *left = source;
    *right = slow->prox;
    slow->prox = NULL;
}

// Função para fundir duas sublistas ordenadas
ListSorted *ListSortedMerge(ListSorted *left, ListSorted *right)
{
    if (left == NULL)
        return right;
    if (right == NULL)
        return left;
    ListSorted *result = NULL;
    if (left->fitness <= right->fitness)
    {
        result = left;
        result->prox = ListSortedMerge(left->prox, right);
    }
    else
    {
        result = right;
        result->prox = ListSortedMerge(left, right->prox);
    }
    return result;
}