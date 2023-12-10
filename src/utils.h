#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// Função para remover espaços em branco do início e do fim da string
void Strip(char *str);

// Função para gerar uma cópia de uma string alocando-a dinâmicamente
char *DuplicateSTR(char *str);

// Função para calcular a distância euclidiana
double DistanciaEuclidiana(int x1, int y1, int x2, int y2);

// Gerar float aleatorio entre 0.0 e 1.0
float GerarFloatAleatorio();

// Gerar numero inteiro aleatorio
int GerarInteiroAleatorio(int min, int max);

// Funcao para extrair (chave:valor) de string lida de arquivo
void ExtractStringKeyValue(char *string_lida, char **key, char **value);

// Trocar dois elementos em um vetor de inteiros
bool CheckInArray(int *array, int size, int value);

// Empurrar posicao x1 de um vetor de inteiros até x0
void VetIntMoveAndInsert(int vetor[], int tamanho, int x1, int x0);

#endif